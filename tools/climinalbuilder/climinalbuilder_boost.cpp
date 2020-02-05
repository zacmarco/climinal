#ifdef INCLUDE_BOOST
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <cstdio>
#include <string>

#include "clistruct.h"

using boost::property_tree::ptree;

namespace {
    void parse_param(ptree&);
    void parse_command(ptree&);
    void parse_context(ptree&);
    void parse_file(ptree&);

    void parse_param(ptree &param)
    {
        std::string name, description, required, numval, values;
        try {
            name=param.get_child("name").get_value<std::string>();
        } catch (std::exception const& e) {}
        try {
            description=param.get_child("description").get_value<std::string>();
            boost::erase_all(description, "\"");
        } catch (std::exception const& e) {}

        required=param.get_child("required").get_value<std::string>();

        try {
            numval=param.get_child("numval").get_value<std::string>();
        } catch (std::exception const& e) {}
        try {
            values=param.get_child("values").get_value<std::string>();
        } catch (std::exception const& e) {}

        //std::cout << "PARAM" << std::endl;
        //std::cout << "NAME:        "  << name << std::endl;
        //std::cout << "DESCRIPTION: "  << description << std::endl;
        //std::cout << "REQUIRED:    "  << required << std::endl;
        //std::cout << "NUMVAL:      "  << numval << std::endl;
        //std::cout << "VALUES:      "  << values << std::endl;
        //std::cout << std::endl; 
        create_param((name.length())?name.c_str():NULL, (description.length())?description.c_str():NULL, (required=="true")?1:0, (numval.length())?std::stoi(numval):1, (values.length())?values.c_str():NULL);

        //std::cout << "END PARAM: " << name << std::endl; 
        end_param();
    }

    void parse_command(ptree &command)
    {
        std::string name(command.get_child("name").get_value<std::string>());
        std::string brief(command.get_child("brief").get_value<std::string>());
        std::string help(command.get_child("help").get_value<std::string>());
        std::string function(command.get_child("function").get_value<std::string>());
        
        /* This is to keep compatibility with bison version of xml parser */
        boost::erase_all(brief, "\"");
        boost::erase_all(help,  "\"");

        //std::cout << "Command" << std::endl;
        //std::cout << "NAME: "  << name << std::endl;
        //std::cout << "BRIEF: " << brief << std::endl;
        //std::cout << "HELP: "  << help << std::endl;
        //std::cout << "FUNC: "  << function << std::endl;
        //std::cout << std::endl; 
        create_command(name.c_str(), brief.c_str(), help.c_str(), function.c_str());

        /* We need unfortunately to do this loops in sequence to keep the events order */
        for ( auto & subnode : command) {
            // Get the label of the node
            if(subnode.first=="param") {
                parse_param(subnode.second);
            }
        }
        for ( auto & subnode : command) {
            // Get the label of the node
            if(subnode.first=="command") {
                parse_command(subnode.second);
            }
        }
        for ( auto & subnode : command) {
            // Get the label of the node
            if(subnode.first=="context") {
                parse_context(subnode.second);
            }
        }

        //std::cout << "END CMD: " << command.get_child("name").get_value<std::string>() << std::endl; 
        end_command();
    }

    void parse_context(ptree &tree)
    {
        std::string prompt=tree.get_child("prompt").get_value<std::string>();
        //std::cout << "Context" << std::endl;
        //std::cout << "PROMPT: " << prompt << std::endl;
        //std::cout << std::endl; 
        create_context(prompt.c_str());
        for ( auto & node : tree) {
            // Get the label of the node
            if(node.first=="command") {
                parse_command(node.second);
            }

        }
        //std::cout << "END CTX: " << tree.get_child("prompt").get_value<std::string>() << std::endl; 
        end_context();
    }

    void parse_file(ptree &tree)
    {
        ptree &ctx_tree = tree.get_child("context");
        parse_context(ctx_tree);
    }

}
int main_boost(int argc, const char **argv)
{
    int err=0;
    ptree tree;
    try {
        read_json(argv[1], tree);
        std::cout << "JSON file detected..." << std::endl;
    } catch (std::exception const& e) {
        err=1;
    }
    if(err) {
        err=0;
        try {
            read_xml(argv[1], tree);
            std::cout << "XML file detected..." << std::endl;
        } catch (std::exception const& e) {
            err=1;
        }
        
    }
    if(!err) {
        try {
            parse_file(tree);
            write_files(argv[2]);
        } catch (std::exception const& e) {
            err=1;
            std::cerr << e.what() << std::endl;
        }

    } else {
        std::cerr << "Not a valid XML or JSON file" << std::endl;
    }

    if(!err) {
        std::cout << "DONE!" << std::endl;
    }

    return err;
}
#endif
