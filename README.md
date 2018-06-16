# csvpp

**csvpp** is a header only library for CSV parsing. It is written in modern C++ and proudly adhering to [RFC4180](https://tools.ietf.org/html/rfc4180).

Main features:
* ready to be used in asynchronous programming context,
* separator character can be chosen or determined automatically,
* it can work with unicode files,
* it can work with binary files.

## Getting started

**csvpp** is an header only library, so you have only to:

 * include `csv.hpp` in your project,
 * call `parse_csv` function. 

`csv_line` takes two parameters:
 * an *input stream*: the only requirement is that the stream derives from [std::basic_istream](http://it.cppreference.com/w/cpp/io/basic_istream),
 * a callback function.

It offers a callback that is invoked whenever a line is read in the CSV file. The callback accepts a string vector as a parameter, and returns a bool. Through the return value of the callback it is possible to stop the processing of the file, which will resume from the point where you stopped in case of new call. You can read the CSV asynchronously!

It's possible to define the callback with:
 * modern lambda functions,
 * more traditional C functions,
 * structures that have overridden the operator `()`.

> **CSV in examples**
> The following code snippets use a CSV taken from [Wikipedia](https://en.wikipedia.org/wiki/Comma-separated_values):
> ```cpp
> static std::string cars = "Year,Make,Model,Description,Price\n"
>    "1997,Ford,E350,\"ac, abs, moon\",3000.00\n"
>    "1999,Chevy,\"Venture \"\"Extended Edition\"\"\",\"\",4900.00\n"
>    "1999,Chevy,\"Venture \"\"Extended Edition, Very Large\"\"\",, 5000.00\n"
>    "1996,Jeep,Grand Cherokee,\"MUST SELL!\n"
>    "air, moon roof, loaded\",4799.00";
> ```

### First Example

```cpp
auto input = std::stringstream(cars); // a generic std :: basic_istream is also allowed
size_t row = 0;                    // a simple counter
parse_csv(input, [&](auto& fields) {
    std::cout << row++ << " | ";
    for (size_t col = 0; col < fields.size(); ++col)
        std::cout << fields[col] << "\t";

    std::cout << std::endl;
    return true; // it's possible to stop the processing returning false
});
```

### Header parsing example

You can manually read a single line of csv using `parse_csv_line` function. For example it's usefull when you have to parse header line. `parse_csv_line` is similar to `parse_csv`, it takes the same parameter except callback function and it return a vector of strings.

```cpp
// ... 
// extract from example.cpp

auto header = parse_csv_line(input, delimiter);
parse_csv(input, delimiter, [&](auto& fields) {
    car local;
    for (size_t index = 0; index < fields.size(); ++index)
        assemble_car(local, header[index], fields[index]);

    garage.push_back(local);
    return true;
});
// ...
```

