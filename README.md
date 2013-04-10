# Protean #

## Description ##

*Protean* is a library for representing hierarchical data in a uniform way
across multiple platforms.

The library defines *a variant object* that can hold primitive data as well as
collections (e.g. lists, dictionaries) with arbitrary depth. The variant can be
inspected, enumerated, and/or serialised to XML/binary format.

Protean has implementations in `C++` and `C#`.

## Compilation ##

Unfortunately, compilation methods are currently mostly untested. We plan to add
further methods later. If you figure out another method not yet described here,
please submit a pull request.

One method, which is known to work on Windows, is the following:

* Install [Boost](http://www.boost.org/). (Known to work with version `1.53.0`.)
* Install [Xerces](http://xerces.apache.org/). (Known to work with version
  `3.1.1`.)
* Edit `projects/vc1x-paths.props`, setting `BOOST_VERSION`, `BOOST_ROOT`, and
  `XERCES_ROOT` appropriately.
* Open `projects/vc1x-protean.sln` in Visual Studio 2010/2012.
* Choose a configuration, e.g. `vc110 Debug` and `x64`, and Build Solution.

## Usage examples ##

### Generating and outputting XML ###

    using namespace protean;
    variant root(variant::Dictionary);
    variant books(variant::Dictionary);
    variant book(variant::Dictionary);
    variant book_attrs(variant::Dictionary);
    book_attrs.insert("author", variant("XYZ"));
    book_attrs.insert("year", variant(2013));
    book.insert(xml_attributes, book_attrs);
    book.insert(xml_text, variant("some text"));
    books.insert("book", book);
    root.insert("books", books);
    std::stringstream contents;
    xml_writer xw(contents, xml_mode::Preserve | xml_mode::Indent);
    xw << root;
    std::cout << contents.str() << std::endl;

This will output:

    <?xml version="1.0" encoding="utf-8"?>
    <books>
      <book author="XYZ" year="2013">some text</book>
    </books>

## Contributing ##

We welcome constructive feedback. Please make use of GitHub's issue tracking and
pull requests.