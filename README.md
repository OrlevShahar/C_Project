# C_Project
This project is the final task of a C course at the Open University, where the objective was to build a compiler for an assembly language provided by the course.

Introduction:
This compiler is designed to process assembly language files, generating machine code in a base-4 encoded format. The project comprises several source files and headers and C files:

    utils.h, utils.c: Contains utility functions.
    macro.h, macro.c: Functions for deploying macros.
    data_table.h, data_table.c: Data structures and functions for handling label tables.
    data_structur.h, data_structur.c: Defines and populates the main data structures used throughout the project.
    preprocessor.h, preprocessor.c: Functions responsible for the preprocessing phase.
    first_run.h, first_run.c: Functions for the first pass of the compiler.
    second_run.h, second_run.c: Functions for the second pass of the compiler.

Data Structures:
The project utilizes the following data structures, defined in data_structur.h:

    address_definition: Stores information about operands, including immediate values, direct addresses, index values, and register addresses.
    line_structur: Represents information about each line in the assembly file, including warnings, label names, and line types (comment, directive, instruction, or definition).

Compilation Process:
The compilation process comprises two passes:

    First Run (first_run): Reads the assembly file, determines the types of each line, updates instruction and data counters (IC and DC), and builds memory images and label tables.

    Second Run (second_run): Processes the assembly file again, updates memory images with label addresses, and handles entry declarations.

Preprocessor:
The preprocessor, defined in preprocessor.h, handles preprocessing steps such as macro expansion.
