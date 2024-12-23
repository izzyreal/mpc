#pragma once

#include <string>
#include <unordered_map>
#include <map>
#include <vector>
#include <array>
#include <algorithm>

namespace mpc::controls::keyboard {

    class KeyHelper
    {
        public:
            using name_to_code_table_t = std::unordered_map<std::string, const int>;
            using code_to_name_table_t = std::unordered_multimap<int, const std::string>;

            using iterator_t = name_to_code_table_t::iterator;
            using const_iterator_t = name_to_code_table_t::const_iterator;

            static const KeyHelper& instance()
            {
                static KeyHelper keyHelper;
                return keyHelper;
            }

            int code(const std::string& name) const
            {
                return name_to_code_table.at(name);
            }

            std::string name(const int code) const
            {
                return code_to_name_table.equal_range(code).first->second;
            }

            std::vector<std::string> names(const int code) const
            {
                std::vector<std::string> result;

                for (auto &e : code_to_name_table)
                {
                    if (e.first != code) continue;
                    result.push_back(e.second);
                }

                std::sort(result.begin(), result.end());
                return result;
            }

            bool is_valid(const int code) const
            { return (code >= 0) && (code_to_name_table.count(code) > 0); }

            iterator_t begin() { return name_to_code_table.begin(); }
            iterator_t end() { return name_to_code_table.end(); }
            const_iterator_t cbegin() const { return name_to_code_table.cbegin(); }
            const_iterator_t cend() const { return name_to_code_table.cend(); }

            std::map<std::string, const int> data_sorted_by_name() const
            { return std::map<std::string, const int>(cbegin(), cend()); }

            std::vector<std::pair<int, std::string>> data_sorted_by_code() const
            {
                using return_element_t = std::pair<int, std::string>;
                using return_t = std::vector<return_element_t>;

                auto by_name = data_sorted_by_name();
                using by_name_element_t = decltype(by_name)::value_type;
                return_t vector;

                std::transform(std::begin(by_name), std::end(by_name), std::back_inserter(vector),
                        [](const by_name_element_t& p)
                        { return return_element_t(p.second, p.first); }
                        );

                std::stable_sort(std::begin(vector), std::end(vector),
                        [](const return_element_t& a, const return_element_t& b)
                        { return a.first < b.first; }
                        );

                return vector;
            }

        private:
            name_to_code_table_t name_to_code_table;
            code_to_name_table_t code_to_name_table;

            static const std::vector<const std::pair<const std::string, const int>> keys;

            KeyHelper()
            {
                for(const auto& k : keys)
                {
                    name_to_code_table.emplace(std::get<0>(k), std::get<1>(k));
                    code_to_name_table.emplace(std::get<1>(k), std::get<0>(k));
                }

            }
    };
} // namespace mpc::controls::keyboard

