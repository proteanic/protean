# Protean #

## Description ##

`protean` is a library for representing hierarchical data in a uniform way across multiple platforms.

The library defines a `variant` object that can hold primitive data as well as collections (e.g.
lists, dictionaries) with arbitrary depth. The variant can be inspected, enumerated, and/or serialised
to XML/binary format.

Protean has implementations in C++, C# and Python.

## Compilation ##

### C++ ###

Protean is known to build on:

- Windows with MS Visual Studio
- Mac with cmake and Apple clang
- CentOS Linux with cmake and GCC or clang

On Windows, the following method will work:

- Install [Boost](http://www.boost.org) 1.69 or later
- Install [Xerces](http://xerces.apache.org) 3.1 or later
- Copy _projects/vc1x/vc1x-paths.props.template_ to _projects/vc1x/vc1x-paths.props_ and edit
  the latter to set `BOOST_VERSION`, `BOOST_ROOT`, and `XERCES_ROOT` appropriately
- Open _projects/vc1x-protean.sln_ in your chosen version of Visual Studio
- Choose a configuration, e.g. `vc142 Release` and `x64`, and Build Solution

On other platforms:

- Install `boost`, `cmake` and `xerces` dependencies, e.g. via `yum` or `apt-get`
- Build via `mkdir build && cd build && cmake .. && make`, or something like `cmake -G Ninja && ninja` to
  use a build tool besides the default of `make`
- Static linking of Boost can be forced via `-DBOOST_STATIC_LINK_PATH=...` if required

## Usage examples ##

### Generating and outputting XML ###

```cpp
using variant = protean::variant;
variant root(variant::Dictionary);
variant books(variant::Dictionary);
variant book(variant::Dictionary);
variant book_attrs(variant::Dictionary);
book_attrs.insert("author", variant("XYZ"));
book_attrs.insert("year", variant(2013));
book.insert(protean::xml_attributes, book_attrs);
book.insert(protean::xml_text, variant("some text"));
books.insert("book", book);
root.insert("books", books);
std::stringstream contents;
protean::xml_writer writer(contents, xml_mode::Preserve | xml_mode::Indent);
writer << root;
std::cout << contents.str() << std::endl;
```

This will output:

```xml
<?xml version="1.0" encoding="utf-8"?>
<books>
  <book author="XYZ" year="2013">some text</book>
</books>
```

## Contributing ##

We welcome contributions and feedback. Please make use of GitHub's issue tracking and pull requests.
