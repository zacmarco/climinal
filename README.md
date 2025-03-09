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

# Command Tree

Climinal structures CLI commands in a hierarchical tree, where:

- **Contexts** define groups of commands
- **Commands** execute specific actions
- **Parameters** define arguments for commands
- **Subcommands** specializations of commands that allow further input customization
  
## Generalized Command Tree Structure

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

# Contexts, Commands, Subcommands, and Parameters in Climinal

Climinal organizes a CLI using a structured **command tree**. This hierarchical tree consists of **contexts**, **commands**, **subcommands**, and **parameters**, each playing a crucial role in defining how the CLI operates. To properly design a CLI with Climinal, it is essential to understand their relationships, their fields, and how to navigate between them effectively.

---

## Contexts
### Definition
A **context** in Climinal defines a **CLI scope** where a specific set of commands is available. The root of the command tree is a special context from which all other commands originate. Additional contexts are created when commands lead to new areas of the CLI, essentially scoping commands in a meaningful way.

### Fields Defining a Context
Each context is specified in the XML or JSON file with fields that define its properties. Based on `testtool.xml`, a context has:
- **A name**: Identifies the context uniquely.
- **A prompt**: Defines what the CLI prompt looks like when inside this context.
- **Commands**: Specifies the set of commands available in this context.

### Relationship with Commands
Each **context** contains **commands**, but **a command can be the parent of only one context**. If a command transitions into a new context, that context will be activated after the command executes.

### Example of Contexts in XML
```xml
<context>
    <name>main</name>
    <prompt>main</prompt>
    <command>
        <name>network</name>
        <context>network</context>
    </command>
</context>

<context>
    <name>network</name>
    <prompt>network</prompt>
    <command>
        <name>set_ip</name>
        <param>
            <name>address</name>
            <required>true</required>
        </param>
    </command>
</context>
```
### Navigation Between Contexts
- When in `main`, entering `network` moves the CLI into the `network` context.
- The prompt changes from `main>` to `network>`, and only the commands in the `network` context become available.

---

## Commands
### Definition
A **command** represents an **action** a user can execute in a given context. It is the primary way of interacting with the CLI and may:
1. Execute an action directly.
2. Have **parameters** that modify its behavior.
3. Lead to a **new context**.
4. Contain **subcommands** for further specialization.

### Fields Defining a Command
- **A name**: The keyword the user types to trigger the command.
- **An optional associated context**: If present, entering this command moves the user into a new CLI context.
- **Optional parameters**: The data inputs required for execution.

### Example of Commands in XML
```xml
<command>
    <name>list</name>
</command>

<command>
    <name>set</name>
    <param>
        <name>value</name>
        <required>true</required>
    </param>
</command>
```
- `list` runs with no additional input.
- `set` requires a `value` parameter.

---

## Subcommands
### Definition
A **subcommand** is simply a **command nested inside another command**. Subcommands allow specialization of a command instead of using parameters.

### Fields Defining a Subcommand
Since a **subcommand is just a command**, it has the same fields as a command:
- **A name**
- **An optional context**
- **Optional parameters**
- **Optional further subcommands**

### When to Use a Subcommand vs. a Parameter
| Use Case | Subcommand | Parameter |
|----------|-----------|-----------|
| **When the command has distinct behaviors requiring different logic** | ✅ Yes | ❌ No |
| **When the command takes a value as input** | ❌ No | ✅ Yes |
| **When auto-completion should show clear options** | ✅ Yes | ❌ No |
| **When the command needs further nesting for future extension** | ✅ Yes | ❌ No |

#### Example: Setting an IP Address
| Approach | Command & Parameter | Command & Subcommand |
|----------|----------------------|----------------------|
| **Syntax** | `set_ip 192.168.1.1` | `network set ip 192.168.1.1` |
| **Structure** | `set_ip` with a required parameter `address` | `set` command with `ip` as a subcommand |
| **When to Use** | If `set_ip` is a standalone command | If `set` has multiple subcommands like `set dns`, `set gateway` |

### Example of Subcommands in XML
```xml
<command>
    <name>set</name>
    <command>
        <name>ip</name>
        <param>
            <name>address</name>
            <required>true</required>
        </param>
    </command>
</command>
```
- `set` is a general command.
- `ip` is a subcommand under `set`, requiring an `address` parameter.

---

## Parameters
### Definition
A **parameter** provides input values to a command. Some parameters are required, while others are optional.

### Fields Defining a Parameter
- **A name**: The name used in the tree definition.
- **Required flag**: Specifies if the parameter is mandatory.
- **Optional callback**: Allows dynamic value generation for auto-completion.

### Example of Parameters in XML
```xml
<command>
    <name>add_user</name>
    <param>
        <name>username</name>
        <required>true</required>
    </param>
</command>
```
- `add_user` requires a `username` parameter.

---

## Relations Between Contexts, Commands, Subcommands, and Parameters
The **command tree** follows these rules:
1. **A context contains commands.**
2. **A command may have parameters, subcommands, or transition to a new context.**
3. **A command can be the parent of only one context.**
4. **A command with subcommands may have parameters as well.**
5. **A parameter belongs to only one command.**
6. **A required parameter must always be specified by the user.**

---

## Example: Designing a CLI for a Smart Home System
### Context Design
- `home>` (Root)
  - `device` → Leads to the `device>` context.
  - `settings` → Leads to the `settings>` context.

- `device>`
  - `list` → Lists all devices.
  - `control`
    - `on <device_id>` → Turns a device on.
    - `off <device_id>` → Turns a device off.

- `settings>`
  - `wifi` → Leads to `wifi>` context.
  - `mode`
    - `eco` → Enables eco mode.
    - `performance` → Enables performance mode.

### XML Definition
```xml
<context>
    <name>home</name>
    <prompt>home</prompt>
    <command>
        <name>device</name>
        <context>device</context>
    </command>
    <command>
        <name>settings</name>
        <context>settings</context>
    </command>
</context>
```

---

## Summary
- **Contexts define available commands and prompt styles.**
- **Commands are actions within a context.**
- **Subcommands specialize a command instead of using parameters.**
- **Parameters provide input values.**
- **Using subcommands makes sense when different logic is required.**
- **Using parameters makes sense when a command takes a value.**

This structure ensures **clarity**, **scalability**, and **ease of use** in CLI design. 🚀


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
climinalbuilder <input_file> <output_name>
```
Files Generated:
- ***<output_name>*.c:**: Contains the tree structure.
- ***<output_name>*.h**: Contains the entry point signature and callback function signatures.

### Main Program and Running the CLI
In the main program, the climinal_main function is called to start the CLI. Here’s an example from nettool_main.c:
```c
climinal_main(stdin, stdout, climinalhandle_<output_name>, priv_data);
```
Where:
- **stdin:** Input stream for receiving commands.
- **stdout:** Output stream for printing responses.
- **climinalhandle_*<output_name>***: This is the autogenerated function from CliminalBuilder, responsible for initializing the command structure.
- **priv_data:** Custom data passed to callbacks, useful for managing state across multiple instances of the CLI


### Implementing Callbacks
Create a C file (e.g. `mycli_cbk.c`) to implement command logic.
Please refer to previous session for callback signatures and implementation details.


### Compiling Your CLI
```sh
cc -o mycli mycli.c mycli_cbk.c mycli_main.c -lcliminal -lm
```

### Running Your CLI
```sh
./mycli
```

# History Management in Climinal

Climinal provides built-in command history management, similar to how history works in **BASH**. This allows users to recall, navigate, and reuse previously executed commands efficiently.

---

## Listing Command History

The **`history`** command is implicitly available in every context of a Climinal-based CLI. When executed, it displays a numbered list of previously executed commands, starting from the oldest to the most recent.

### Example Usage

#### User Input:
```sh
main> history
```

#### Example Output:
```sh
  1  info system
  2  info all
  3  credits
  4  set mode automatic
  5  show status
```

Each command is listed with an associated sequence number. This number can be used to recall and re-execute a specific command.

---

## Recalling Commands from History

Like in **BASH**, Climinal allows recalling a specific command using the **"!"** symbol followed by the sequence number from the history list.

### Example Usage

#### User Input:
```sh
main> !3
```

#### Equivalent to:
```sh
main> credits
```

This will execute the **`credits`** command again, just as if the user had typed it manually.

---

## Re-executing the Last Command

To re-execute the most recently executed command, the user can use:
```sh
main> !!
```
This works the same way as in **BASH** and executes the last command in history.

### Example Usage

#### User Input:
```sh
main> !!
```

#### Equivalent to:
If the last executed command was:
```sh
main> show status
```
Then **`!!`** will execute it again:
```sh
main> show status
```

---

## Navigating Through History Using Arrow Keys

Like **BASH**, Climinal supports command history navigation using the **Up** and **Down** arrow keys.

- **Pressing the Up arrow (`↑`)** retrieves the previous command in history.
- **Pressing the Down arrow (`↓`)** moves forward through the history, towards more recent commands.

### Example Usage

1. Suppose the command history contains:
   ```sh
   1  info system
   2  set mode automatic
   3  show status
   ```
2. If the user is at the prompt and presses **Up (`↑`)** once, the command line will change to:
   ```sh
   main> show status
   ```
3. Pressing **Up (`↑`)** again:
   ```sh
   main> set mode automatic
   ```
4. Pressing **Down (`↓`)** will return to **show status**.
5. Pressing **Down (`↓`)** again will return to an empty command line, indicating that the user has reached the most recent point in history.

This feature allows users to efficiently recall and modify previous commands without retyping them.

---

## Using History for Efficient CLI Interaction

Using history features, a user can quickly repeat or modify previous commands without retyping them. This is particularly useful for:
- Debugging commands in a long CLI session.
- Running frequent or complex commands multiple times.
- Quickly switching between different configurations or modes.

---

## Example Full Session Using History

```sh
main> info system
System Information:
  OS: Linux
  Kernel: 5.15.0
  Architecture: x86_64

main> set mode automatic
Mode set to: Automatic

main> history
  1  info system
  2  set mode automatic

main> !1
System Information:
  OS: Linux
  Kernel: 5.15.0
  Architecture: x86_64

# User presses Up (`↑`) key
main> set mode automatic  # Previous command appears

# User presses Up (`↑`) key again
main> info system  # Older command appears
```

---

### Summary of History Commands in Climinal

| Command | Description |
|---------|------------|
| `history` | Displays the list of previously executed commands with numbers. |
| `!<num>` | Executes the command with the specified history number. |
| `!!` | Executes the last executed command again. |
| `↑ (Up Arrow)` | Scrolls backward through command history. |
| `↓ (Down Arrow)` | Scrolls forward through command history. |

These history features make Climinal's CLI environment much more user-friendly and efficient for interactive command execution.

# Conclusion
Climinal provides a structured way to build interactive CLI applications with auto-completion, strict validation, and hierarchical contexts. 

For more details, visit the [GitHub repository](https://github.com/zacmarco/climinal).
