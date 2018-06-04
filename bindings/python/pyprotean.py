"""
Python conversion functions for reading from and writing to
protean variants

Protean Project: https://github.com/proteanic/protean

Notes: 
 * Date, Float and Object protean types not supported (types are likely to be dropped from protean)
 * Unsigned types need further testing
 * Test Buffer type (is 'c' the right struct format code?)
 * Test arrays
   * Support reading/writing of array.array (for compatible types) and/or numpy (if present)? 
 * Test exception reading/writing
 * Bug in time microsecond conversion

"""


# BEGIN: Python 3 compatability
from __future__ import print_function
from __future__ import division
from __future__ import unicode_literals

from future_builtins import *
# END: Python 3 compatability


import zlib
import struct
import cStringIO
import time
import math
import datetime
import collections


_protean_binary_magic_number = 0x484913FF
_protean_major_version = 1
_protean_minor_version = 1

_xdr2py_registry = {}
_py2xdr_registry = {}

# Wrapper classes for explicit conversion of values to unsigned types
class UInt32(int): pass
class UInt64(long): pass

class PyProteanException(Exception):
    pass

class Register(object):
    def __init__(self, protean_types, pytype2variantType = None):
        super(Register, self).__init__()
        self.protean_types = protean_types
        (self.py_type, self.py2xdr_type) = pytype2variantType if pytype2variantType else (None, None)

    def __call__(self, cls):
        for ptyp in self.protean_types:
            _xdr2py_registry[ptyp] = cls
        if self.py_type:
            cls.py2xdr_type = self.py2xdr_type
            _py2xdr_registry[self.py_type] = cls
        return cls

class SetFormatTypeAndRegister(Register):
    def __init__(self, protean_types, (format_type, type_size), py_types = None):
        super(SetFormatTypeAndRegister, self).__init__(protean_types, py_types)
        self.format_type = format_type
        self.type_size = type_size

    def __call__(self, cls):
        super(SetFormatTypeAndRegister, self).__call__(cls)
        cls.format_type = self.format_type
        cls.type_size = self.type_size
        return cls

class _ProteanSimpleTypeBase(object):
    @classmethod
    def _convert_from_py(cls, v):
        return v

    @classmethod
    def _convert_to_py(cls, v):
        return v

    @classmethod
    def read(cls, data):
        v = struct.unpack(cls.format_type, data.read(cls.type_size))[0]
        return cls._convert_to_py(v)

    @classmethod
    def write(cls, data, value):
        data.write(struct.pack(cls.format_type, cls._convert_from_py(value)))

@Register([0x00000001], (None, 0x00000001))
class pyprotean_None(object):
    @classmethod
    def read(cls, data):
        return None

    @classmethod
    def write(cls, data, value):
        assert value is None

@Register([0x00000002, 0x00000004], (str, 0x00000004))
class pyprotean_String(object):
    @classmethod
    def read(cls, data):
        strlen  = pyprotean_UInt32.read(data)
        padding = (4 - (strlen % 4)) % 4
        v = struct.unpack(b'c'*(strlen+padding), data.read(strlen+padding))
        v = b''.join(v[:strlen])
        return v

    @classmethod
    def write(cls, data, value):
        pyprotean_UInt32.write(data, len(value))
        padding = (4 - (len(value) % 4)) % 4
        data.write(struct.pack(b'c'*(len(value)+padding), *list(value+b'\0'*padding)))

@SetFormatTypeAndRegister([0x00000008], (b'i', 4), (bool, 0x00000008))
class pyprotean_Bool(_ProteanSimpleTypeBase):
    @classmethod
    def _convert_to_py(cls, v):
        return bool(v)

@SetFormatTypeAndRegister([0x00000010], (b'i', 4), (int, 0x00000010))
class pyprotean_Int32(_ProteanSimpleTypeBase):
    pass


@SetFormatTypeAndRegister([0x00000020], (b'I', 4), (UInt32, 0x00000020))
class pyprotean_UInt32(_ProteanSimpleTypeBase):
    @classmethod
    def _convert_to_py(cls, v):
        return UInt32(v)

@SetFormatTypeAndRegister([0x00000040], (b'q', 8), (long, 0x00000040))
class pyprotean_Int64(_ProteanSimpleTypeBase):
    pass

@SetFormatTypeAndRegister([0x00000080], (b'Q', 8), (UInt64, 0x00000080))
class pyprotean_UInt64(_ProteanSimpleTypeBase):
    @classmethod
    def _convert_to_py(cls, v):
        return UInt64(v)

@SetFormatTypeAndRegister([0x00000200], (b'd', 8), (float, 0x00000200))
class pyprotean_Double(_ProteanSimpleTypeBase):
    pass

@Register([0x00000800], (datetime.time, 0x00000800))
class pyprotean_Time(object):
    @classmethod
    def read(cls, data):
        v = pyprotean_Int64.read(data)
        rm, mssc = divmod(v/1000, 60)
        ms, sc = math.modf(mssc)
        hr, mn = divmod(rm, 60)
        return datetime.time(int(hr), int(mn), int(sc), microsecond=int(ms*1000))

    @classmethod
    def write(cls, data, value):
        v = (value.hour*60*60+value.minute*60+value.second)*1000 + value.microsecond
        pyprotean_Int64.write(data, v)
        

@Register([0x00001000], (datetime.datetime, 0x00001000))
class pyprotean_DateTime(object):
    epoch_start = 17987443200000L
    @classmethod
    def read(cls, data):
        v = pyprotean_Int64.read(data)
        ts = time.gmtime((v - cls.epoch_start) / 1000)
        return datetime.datetime(ts.tm_year, ts.tm_mon, ts.tm_mday, ts.tm_hour, ts.tm_min, ts.tm_sec)

    @classmethod
    def write(cls, data, value):
        v = time.mktime(value.timetuple()) * 1000 + cls.epoch_start
        pyprotean_Int64.write(data, v)


class _ProteanSequenceType(object):
    @classmethod
    def _convert_to_py(cls, v):
        return v

    @classmethod
    def _convert_from_py(cls, v):
        return v

    @classmethod
    def read(cls, data):
        seqlen = pyprotean_UInt32.read(data)
        entries = [ read_variant(data) for _ in range(seqlen) ]
        return cls._convert_to_py(entries)

    @classmethod
    def write(cls, data, value):
        pyprotean_UInt32.write(data, len(value))
        [ write_variant(data, v) for v in cls._convert_from_py(value) ]

@Register([0x00002000], (list, 0x00002000))
class pyprotean_List(_ProteanSequenceType):
    @classmethod
    def _convert_to_py(cls, v):
        return list(v)

@Register([0x00020000], (tuple, 0x00020000))
class pyprotean_Tuple(_ProteanSequenceType):
    @classmethod
    def _convert_to_py(cls, v):
        return tuple(v)

@Register([0x00004000, 0x00008000], (dict, 0x00004000))
class pyprotean_Mapping(object):
    @classmethod
    def _convert_to_py(cls, v):
        return dict(v)

    @classmethod
    def _convert_from_py(cls, v):
        return v.items()
    
    @classmethod
    def read(cls, data):
        seqlen = pyprotean_UInt32.read(data)
        entries = []
        for _ in range(seqlen):
            entries.append( (pyprotean_String.read(data), read_variant(data)) )
        return cls._convert_to_py(entries)

    @classmethod
    def write(cls, data, value):
        pyprotean_UInt32.write(data, len(value))
        for k, v in cls._convert_from_py(value):
            pyprotean_String.write(data, k)
            write_variant(data, v)

@Register([0x00080000])
class pyprotean_TimeSeries(object):
    @classmethod
    def _convert_to_py(cls, v):
        return v

    @classmethod
    def _convert_from_py(cls, v):
        return v

    @classmethod
    def read(cls, data):
        seqlen = pyprotean_UInt32.read(data)
        entries = []
        for _ in range(seqlen):
            entries.append( (pyprotean_DateTime.read(data), read_variant(data)) )
        return cls._convert_to_py(entries)

    @classmethod
    def write(cls, data, value):
        pyprotean_UInt32.write(data, len(value))
        for k, v in cls._convert_from_py(value):
            pyprotean_DateTime.write(data, k)
            write_variant(data, v)

@Register([0x00010000], (buffer, 0x00010000))
class pyprotean_Buffer(object):
    @classmethod
    def read(cls, data):
        buflen = pyprotean_UInt32.read(data)
        return buffer(data.read(b'c'*len(buflen), buflen))

    @classmethod
    def write(cls, data, value):
        pyprotean_UInt32.write(len(value), data)
        data.write(struct.pack(b'c'*len(value), value))


@Register([0x00040000], (Exception, 0x00040000))
class pyprotean_Exception(object):
    _Protean_Exception_Type = collections.namedtuple('_Protean_Exception_Type', 'type message stack source')
    @classmethod
    def read(cls, data):
        # Read type, message, source, stack and convert to a Python exception object
        exc_type = pyprotean_String.read(data)
        exc_message = pyprotean_String.read(data)
        exc_source = pyprotean_String.read(data)
        exc_stack = pyprotean_String.read(data)
        return cls._Protean_Exception_Type(exc_type, exc_message, exc_stack, exc_source)

    @classmethod
    def write(cls, data, value):
        if not isinstance(value, BaseException):
            raise PyProteanException("Only BaseException type dervied instances can be written as a protean variant")
        pyprotean_String.write(type(value).__name__, data)
        pyprotean_String.write(value.message, data)
        pyprotean_String.write("", data)
        pyprotean_String.write("", data) # TODO: Can fetch stack from sys.last_traceback, call traceback.format_tb first

@Register([0x00200000])
class pyprotean_Array(object):
    @classmethod
    def read(self, data):
        arrlen = pyprotean_UInt32.read(data)
        arrtyp = pyprotean_UInt32.read(data)
        # Makes assumption that all types are identical
        # TODO: Are binary array variants efficient or not? (Store multiple type info or just one?)
        # TODO: If arrtyp == double should we create array.array? or numpy.array?
        return [ pyprotean_Double.read(data) for i in range(arrlen) ]

    @classmethod
    def write(self, data, value):
        # TODO: Write arrays back as list? 
        #       (This function does not get called currently - if array.array or numpy.array were supported, this would/could be called)
        #       Support Python array.array's of doubles?
        #       Support numpy arrays?
        pyprotean_UInt32.write(len(value), data)
        pyprotean_UInt32.write(get_type_encoder(value).py2xdr_type, data)
        for v in value:
            write_variant(data, v)


def read_variant(data):
    typ = pyprotean_UInt32.read(data)

    decoder = _xdr2py_registry.get(typ)
    if not decoder:
        raise PyProteanException("Unable to find a decoder for variant of type {:#x}".format(typ))

    return decoder.read(data)

def get_type_encoder(value):
    encoder = _py2xdr_registry.get(type(value))
    if not encoder:
        raise PyProteanException("Unable to find an encoder for Python object type {}".format(type(value).__name__))
    return encoder

def write_variant(data, value):
    encoder = get_type_encoder(value)

    pyprotean_UInt32.write(data, encoder.py2xdr_type)
    encoder.write(data, value)

    return data

class _binary_mode(object):
    compress = 0x01
    zlib_header = 0x02
    create_proxy = 0x04
    datetime_as_ticks = 0x08

def xdr2py(xdr):
    """
    Build PyProtean variant from xdr

    """
    (xdr_magic, xdr_ver, xdr_mode), xdr = struct.unpack(b'iii', xdr[:12]), xdr[12:]
    if xdr_magic != _protean_binary_magic_number:
        raise PyProteanException("Failed to match protean magic number (expected {:#x}, received {:#x}). Unable to decode data. " \
                            .format(xdr_, _protean_binary_magic_number))
    if ((xdr_ver >> 16) & 0xFFFF) != _protean_major_version:# or (xdr_ver & 0xFFFF) != _protean_minor_version:
        raise PyProteanException("Failed to match protean version number (expected {}.{}, received {}.{}. Unable to decode data. " \
                            .format(_protean_major_version, _protean_minor_version, (xdr_ver >> 16) & 0xFFFF, xdr_ver & 0xFFFF))

    decompress = xdr_mode & _binary_mode.compress
    if decompress:
        no_header = 1 if (xdr_mode & _binary_mode.zlib_header) else -1
        xdr = zlib.decompress(xdr, no_header*zlib.MAX_WBITS)

    xdr = cStringIO.StringIO(xdr)

    return read_variant(xdr)

def py2xdr(pyvalue, compress = True):
    """
    Build PyProtean variant from xdr

    """
    xdr_mode = _binary_mode.datetime_as_ticks # Need to set no header and compress == True/False flags here
    if compress:
        xdr_mode |= _binary_mode.compress
    header = cStringIO.StringIO()
    header.write(struct.pack(b'iii', _protean_binary_magic_number, (_protean_major_version << 16) | _protean_minor_version, xdr_mode))

    data = cStringIO.StringIO()
    write_variant(data, pyvalue)
    if compress:
        # Strip the first two bytes (header) and last four bytes (checksum)
        xdr = zlib.compress(data.getvalue(), -1)[2:-4]
    else:
        xdr = data.getvalue()

    return header.getvalue() + xdr

def xml2py(xml):
    raise NotImplementedError

def py2xml(py):
    raise NotImplementedError

