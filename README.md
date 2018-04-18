# csvpp

![GitHub release](https://img.shields.io/badge/release-0.1.0-blue.svg)

**csvpp** is a header only library for CSV parsing. It is written in modern C++ and proudly adhering strictly to [RFC4180](https://tools.ietf.org/html/rfc4180).

## Examples

### Functional Programming

If you love functional programming, parse_csv is the one for you!
`parse_csv` offers a callback that is invoked whenever a line is read in the CSV file. The callback has only one parameter of type `csv_line&`. The csv_line structure is an inneritance of a string vector, with some methods to support string processing. Through the return value of the callback it is possible to stop the processing of the file, which will resume from the point where you stopped in case of new call. You can read the CSV asynchronously!

It's possible to define the callback with:
 * modern lambda functions,
 * more traditional C functions,
 * structures that have overridden the operator `()`.

> **CSV in examples**
> The following code snippet always use a CSV taken from [Wikipedia](https://en.wikipedia.org/wiki/Comma-separated_values):
> ```cpp
> static std::string cars = "<Year>,Make,Model,Des.cription,P_rice\n"
>	"1997,Ford,E350,\"ac, abs, moon\",3000.00\n"
>	"1999,Chevy,\"Venture \"\"Extended Edition\"\"\",\"\",4900.00\n"
>	"1999,Chevy,\"Venture \"\"Extended Edition, Very Large\"\"\",, 5000.00\n"
>	"1996,Jeep,Grand Cherokee,\"MUST SELL!\n"
>	"air, moon roof, loaded\",4799.00";
> ```
 
 #### Lambda
 
```cpp
auto ss = std::stringstream(cars); // a generic std :: basic_istream is also allowed
size_t row = 0;                    // a simple counter
parse_csv(ss, [&](auto& fields)
{
    std::cout << row++ << " | ";
    for (size_t col = 0; col < fields.size(); ++col)
    {
        std::cout << fields[col] << "\t";
    }
    std::cout << std::endl;
    return true; // it's possible to stop the processing returning false
});
```

#### Simple function

```cpp
bool printer(csv_line<std::string::value_type, std::string::traits_type> & fields)
{
    for (size_t col = 0; col < fields.size(); ++col)
    {
        std::cout << fields[col] << "\t";
    }
    std::cout << std::endl;
    return true; // it's possible to stop the processing returning false
}

int main()
{
    auto ss = std::stringstream(cars); // a generic std :: basic_istream is also allowed
    parse_csv(ss, printer);
}
```

#### Structure

```cpp
struct printer {
    bool operator()(csv_line<std::string::value_type, std::string::traits_type> & fields)
    {
        for (size_t col = 0; col < fields.size(); ++col)
        {
            std::cout << fields[col] << "\t";
        }
        std::cout << std::endl;
        return true;
    }
};

int main()
{
    auto ss = std::stringstream(cars); // a generic std :: basic_istream is also allowed
    printer p;
    parse_csv(ss, p);
}
```