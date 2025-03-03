# Climinal: Interactive CLI Framework

## Overview
Climinal is an open-source software library designed to help developers create interactive command-line interfaces (CLIs) with multiple contexts, custom prompts, auto-completion, and structured command validation. It is particularly well-suited for embedded devices, following a HOST/TARGET model analogous to cross-compilation environments.

- The **HOST** machine is where the CLI is designed and configured.
- The **TARGET** machine is where the CLI runs after deployment.

## Features
- **Hierarchical CLI structure** with nested contexts.
- **Strict command validation** ensuring no ambiguity.
- **Auto-completion** based on predefined structures and runtime callbacks.
- **Context-specific commands**, preventing execution in irrelevant contexts.
- **Lightweight** with no external dependencies (POSIX-compatible).
- **Support for default parameters** and dynamically generated values.
- **Tools for generating C code from XML/JSON definitions.**

## Defining the Command Tree

Climinal structures CLI commands in a hierarchical tree, where:

- **Contexts** define groups of commands
- **Commands** execute specific actions
- **Parameters** define arguments for commands
- **Subcommands** specializations of commands that allow further input customization
- 
# Generalized Command Tree Structure

The CLI command structure is organized in a command tree.
Every command belongs to a context, and the root node of the whole tree is called the **root context**

```
root_context
├── command_1
│   ├── context_1
│   │   ├── command_3
│   │   │   ├── parameter_1
│   │   │   ├── parameter_2
│   │   ├── command_4
│   │       ├── parameter_3
│   ├── command_2
│       ├── context_2
│       │   ├── command_5
│       │       ├── parameter_4
│       │       ├── parameter_5
│       ├── command_6
│           ├── parameter_6
├── command_7
```

# Callbacks implementation

## Command Callback
The command callback in Climinal is defined with the signature:

```c
int cbk(FILE* in, FILE* out, const Cmdinfo *info, const char* line, void *cookie)
```


### Parameters:

- **FILE\* in and FILE\* out:**
These represent the input and output streams, respectively. They allow the callback to read user input and write output directly.

 - **const Cmdinfo \*info:**
This pointer provides metadata about the command being executed. It typically contains details such as the command name, its description, and the parameters expected by the command.

- **const char\* line:**
This is the raw command line entered by the user. It can be parsed further if needed.

- **void \*cookie:**
The cookie argument is a user-defined pointer that can be set when registering the command. It is used to pass additional context or custom data to the callback without altering the function’s signature. For example, you might use it to store configuration data or a pointer to a resource that the callback requires to perform its work.

### Return Value:

The callback returns an int—typically 0 to indicate success or a non-zero value to signal an error.

### Example Callback implementation

```c
#include "climinal.h"

int command_3_callback(int argc, char **argv, void *priv_data) {
    // Get parameter values
    const char *param1 = CLIMINAL_GET_PARAM_VAL(0);
    const char *param2 = (argc > 1) ? CLIMINAL_GET_PARAM_VAL(1) : "default";
    
    printf("Executing command_3 with %s and %s\n", param1, param2);
    return CLIMINAL_NO_ERROR;
}
```

## Value Completion Callback
The value completion callback is defined with the following signature:

```c
int values_cbk(char **val, void *cookie)
```

### Parameters:

- **char \*\*val:**
This is a pointer to an array of strings. The completion callback is expected to fill this array with possible completions for the value currently being typed by the user.

- **void \*cookie:**
Similar to the command callback, the cookie here is a user-defined pointer that allows you to pass additional contextual information to the completion callback. This might include state or configuration details needed to generate the appropriate list of completions.

### Return Value:

The function returns an int—usually representing the number of completions added to the array.

## Navigational Macros
Climinal provides several macros to simplify the retrieval and handling of command parameters within the code of command callbacks. These macros abstract away the underlying details of parameter storage and help maintain a consistent interface. The key macros are:

- **CLIMINAL_GET_DEFVAL:**
Retrieves the default value for a parameter. If the user does not supply a value, this macro is used to fetch what should be used as the default.

- **CLIMINAL_GET_PARAM_NAME:**
Returns the name of a given parameter. This is useful for generating help messages or error messages indicating which parameter is missing or misconfigured.

- **CLIMINAL_GET_PARAM_NUM:**
Obtains the number of parameters expected or provided for a command. This helps in validating whether the correct number of arguments has been supplied by the user.

- **CLIMINAL_GET_PARAM_NUM_VAL:**
Specifically used to retrieve a parameter that is expected to be a numeric value. It typically performs the necessary conversion from string to a numeric type and validates the value.

- **CLIMINAL_GET_PARAM_VAL:**
Retrieves the string value of a command parameter. This macro is used when a parameter is expected to be a textual value.

These macros are designed to reduce boilerplate code when handling command parameters and to ensure that commands have a consistent way of accessing parameter data. They make it easier to write commands that are both robust and user-friendly.

### Error codes

- **CLIMINAL_NO_ERROR** – Command executed successfully
- **CLIMINAL_ERROR_INVALID_PARAM** – Invalid parameter
- **CLIMINAL_ERROR_MISSING_PARAM** – Required parameter missing

# Main Program and Running the CLI
In the main program, the climinal_main function is called to start the CLI. Here’s an example from nettool_main.c:

```c
climinal_main(stdin, stdout, climinalhandle_<output_name>, priv_data);
```

- **stdin:** Input stream for receiving commands.
- **stdout:** Output stream for printing responses.
- **climinalhandle_<output_name>:** This is the autogenerated function from CliminalBuilder, responsible for initializing the command structure.
- **priv_data:** Custom data passed to callbacks, useful for managing state across multiple instances of the CLI.


## Download and Installation
### Cloning the Repository
```sh
git clone https://github.com/zacmarco/climinal.git
cd climinal
```

### Building libcliminal (TARGET library)
#### Requirements
- POSIX-compatible host system
- POSIX-compatible target system
- `gcc` or `clang` compiler (cross compilation)
- `make`
- `cmake`

#### Build Instructions
```sh
mkdir build && cd build
cmake ..
make
sudo make install  # Optional: Install system-wide
```

### Installing on the Target Machine
Since `libcliminal` is meant to run on the **TARGET** device, transfer the compiled library and CLI application using `scp` or `rsync`:
```sh
scp libcliminal.so user@target:/path/to/install/
```

## Building CliminalBuilder (HOST Tool)
**CliminalBuilder** is a tool that runs on the **HOST** machine. It converts XML or JSON CLI definitions into C code.

#### Requirements
- POSIX-compatible host system
- `gcc` or `clang` compiler
- `make`
- `cmake`
- `libboost-dev (optional)`

#### Build Instructions
```sh
cd tools/climinalbuilder
make
```

## Building and Running an Example
Climinal includes example projects in the `examples/` directory. Below is how to build and run `testtool`:

#### Steps
```sh
cd examples/testtool
../../tools/climinalbuilder testtool.xml testtool  # Generate C code
cc -o testtool testtool.c testtool_cbk.c testtool_main.c -lcliminal -lm  # Compile
./testtool  # Run CLI
```

## Defining Your CLI Structure
Climinal uses XML or JSON to define CLI structures. Here’s an example XML structure with multiple contexts and subcommands:

```xml
<context name="root">
    <commands>
        <command name="show">
            <commands>
                <command name="version"/>
                <command name="status"/>
            </commands>
        </command>
        <command name="configure">
            <context name="config">
                <commands>
                    <command name="set">
                        <param>
                            <required>true</required>
                            <values>config_values_cbk</values>
                        </param>
                    </command>
                    <command name="reset"/>
                </commands>
            </context>
        </command>
    </commands>
</context>
```

### Generating the C Code
```sh
climinalbuilder mycli.xml mycli
```

### Implementing Callbacks
Create a C file (`mycli_cbk.c`) to implement command logic:
```c
void show_version_callback(void *priv_data) {
    printf("CLI Version 1.0\n");
}
void configure_set_callback(void *priv_data) {
    printf("Applying configuration\n");
}
```

### Compiling Your CLI
```sh
cc -o mycli mycli.c mycli_cbk.c mycli_main.c -lcliminal -lm
```

### Running Your CLI
```sh
./mycli
```

## Conclusion
Climinal provides a structured way to build interactive CLI applications with auto-completion, strict validation, and hierarchical contexts. 

For more details, visit the [GitHub repository](https://github.com/zacmarco/climinal).
