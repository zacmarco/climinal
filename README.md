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

## Download and Installation
### Cloning the Repository
```sh
git clone https://github.com/zacmarco/climinal.git
cd climinal
```

### Building the Main Library (libcliminal)
#### Requirements
- POSIX-compatible system
- `gcc` or `clang` compiler
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
