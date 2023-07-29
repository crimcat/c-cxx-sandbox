
#include "libcsv/include/csv.h"

#include <iostream>
#include <fstream>
#include <cstring>

/**
 * @brief Print table header - list of column names
 * 
 * @param table CSV table
 */
void print_header(const csv::table<int> &table) {
	std::cout << " ,";
	for(auto it = table.columns().begin(); it != table.columns().end(); ++it) {
		std::cout << " " << *it;
		if(std::distance(it, table.columns().end()) != 1) {
			std::cout << ",";
		}
	}
	std::cout << '\n';
}

/**
 * @brief "Pretty" print of the CSV table
 * Header is printed first, then all rows according to the order they are listed in the source CSV.
 * 
 * @param CSV table
 */
void pretty_print(const csv::table<int> &table) {
	print_header(table);
	for(const auto &r : table.rows()) {
		std::cout << r << ",";
		for(auto c_it = table.columns().begin(); c_it != table.columns().end(); ++c_it) {
			std::cout << " ";
			try {
				std::cout << table.cell_value(r, *c_it);
			} catch(const csv::eval_exception &e) {
				std::cout << e.what();
			}
			if(std::distance(c_it, table.columns().end()) != 1) {
				std::cout << ",";
			}
		}
		std::cout << '\n';
	}	
}

/**
 * @brief Entry point
 * Program accepts one parameter - CSV file name.
 * 
 * @param argc number of command line arguments
 * @param argv array of command line arguments
 * @return execution status - 0 on success
 */
int main(int argc, char *argv[]) {
	if(argc != 2) {
		std::cout << "Usage:\n\t" << argv[0] << " <csv file name>\n";
	} else {
		const bool is_stdin = (0 == std::strcmp("-", argv[1]));
		std::istream *csv_is = is_stdin ? &std::cin : new std::ifstream(argv[1]);
		try {
			csv::table<int> table = csv::table<int>::extract(*csv_is);
			pretty_print(table);
		} catch(const csv::irregular_rows &e) {
			std::cerr << "CSV should have same rows length\n";
		} catch(const csv::bad_or_failed_input_stream &e) {
			std::cerr << "Error reading from " << argv[1] << '\n';
		} catch(const csv::parse_exception &e) {
			std::cerr << "CSV structure parsing failed\n";
		} catch(const csv::implementation_bug &e) {
			std::cerr << "Algorithm implementation bug\n";
		}

		if(!is_stdin) {
			delete csv_is;
		}
	}
	return 0;
}

