#include "csv.hpp"
#include <iostream>
#include <sstream>
#include <tuple>
#include <vector>

using namespace michelemei;

class car
{
public:
	enum class make_enum : unsigned int
	{
		None = 0,
		Ford = 1,
		Chevy = 2,
		Jeep = 3
	};
    static make_enum create_make_enum(std::string const& name)
    {
        if (name.compare("Ford") == 0)
            return make_enum(1);
        if (name.compare("Chevy") == 0)
            return make_enum(2);
        if (name.compare("Jeep") == 0)
            return make_enum(3);
        return make_enum(0);
    }
	static const char* to_string(const make_enum& make) {
		switch (make) {
		    case 1:
                return "Ford";
		    case 2:
                return "Chevy";
		    case 3:
                return "Jeep";
		}
		return "None";
	}

	int year;
	make_enum make = make_enum::None;
	std::string model;
	std::string description;
	double price;
};

static void assemble_car(car& car, std::string name, std::string& value)
{
    // I suggest to use a more elegant (and efficient) compile time alternative:
    // https://stackoverflow.com/questions/16388510/evaluate-a-string-with-a-switch-in-c

    if (name.compare("Year") == 0)
    {
        car.year = atoi(value.c_str());
    }
    else if (name.compare("Make") == 0)
    {
        car.make = car::create_make_enum(value);
    }
    else if (name.compare("Description") == 0)
    {
        car.description = std::move(value);
    }
    else if (name.compare("Price") == 0)
    {
        car.price = atof(value.c_str());
    }
}

int main()
{
    // cars catalog is taken from wikipedia:
    // https://en.wikipedia.org/wiki/Comma-separated_values
    static std::string cars = "Year,Make,Model,Description,Price\n"
        "1997,Ford,E350,\"ac, abs, moon\",3000.00\n"
        "1999,Chevy,\"Venture \"\"Extended Edition\"\"\",\"\",4900.00\n"
        "1999,Chevy,\"Venture \"\"Extended Edition, Very Large\"\"\",, 5000.00\n"
        "1996,Jeep,Grand Cherokee,\"MUST SELL!\n"
        "air, moon roof, loaded\",4799.00";

    // create a stringstream (a generic std::basic_istream is allowed)
    auto ss = std::stringstream(cars);

    // create a container for parsed data
    std::vector<car> garage;

    // discover delimiter (optional)
    char delimiter = discover_delimiter(ss);

    // parse header
    auto header = parse_csv_line(ss, delimiter);

    // parse it
    parse_csv(ss, delimiter, [&](auto& fields) {
        car local;
        for (size_t index = 0; index < fields.size(); ++index)
            assemble_car(local, header[index], fields[index]);

        garage.push_back(local);
        return true;
    });

    std::cout << " --- Garage ---" << std::endl;
    for (const auto& c : garage)
        std::cout << c.price << "\t" << car::to_string(c.make) << "\t" << c.model << std::endl;
	return 0;
}