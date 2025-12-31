/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "CPP-AP", "index.html", [
    [ "MIT License", "md_LICENSE.html", null ],
    [ "Dev Notes", "md_docs_2dev__notes.html", [
      [ "Overview", "index.html#overview", null ],
      [ "Related Pages", "index.html#related-pages", null ],
      [ "Compiler Compatibilty", "index.html#compiler-compatibilty", null ],
      [ "License", "index.html#license", null ],
      [ "Building and Testing", "md_docs_2dev__notes.html#building-and-testing", [
        [ "Build the Testing Executable", "md_docs_2dev__notes.html#build-the-testing-executable", null ],
        [ "Runing the Tests", "md_docs_2dev__notes.html#runing-the-tests", null ]
      ] ],
      [ "Formatting", "md_docs_2dev__notes.html#formatting", null ],
      [ "Documentation", "md_docs_2dev__notes.html#documentation", [
        [ "Prerequisites", "md_docs_2dev__notes.html#prerequisites", null ],
        [ "Generating the Documentation", "md_docs_2dev__notes.html#generating-the-documentation", null ]
      ] ]
    ] ],
    [ "Tutorial", "md_docs_2tutorial.html", [
      [ "Setting Up CPP-AP", "md_docs_2tutorial.html#setting-up-cpp-ap", [
        [ "CMake Integration", "md_docs_2tutorial.html#cmake-integration", null ],
        [ "Bazel Build System", "md_docs_2tutorial.html#bazel-build-system", null ],
        [ "Downloading the Library", "md_docs_2tutorial.html#downloading-the-library", null ]
      ] ],
      [ "The Parser Class", "md_docs_2tutorial.html#the-parser-class", null ],
      [ "Adding Arguments", "md_docs_2tutorial.html#adding-arguments", [
        [ "Syntax", "md_docs_2tutorial.html#syntax", null ],
        [ "Names", "md_docs_2tutorial.html#names", null ],
        [ "Value Types", "md_docs_2tutorial.html#value-types", null ],
        [ "Boolean Flags", "md_docs_2tutorial.html#boolean-flags", null ]
      ] ],
      [ "Argument Parameters", "md_docs_2tutorial.html#argument-parameters", [
        [ "Common Parameters", "md_docs_2tutorial.html#common-parameters", [
          [ "1. <tt>help</tt> - The argument's description which will be printed when printing the parser class instance.", "md_docs_2tutorial.html#autotoc_md1-help---the-arguments-description-which-will-be-printed-when-printing-the-parser-class-instance", null ],
          [ "2. <tt>hidden</tt> - If this option is set for an argument, then it will not be included in the program description.", "md_docs_2tutorial.html#autotoc_md2-hidden---if-this-option-is-set-for-an-argument-then-it-will-not-be-included-in-the-program-description", null ],
          [ "3. <tt>required</tt> - If this option is set for an argument and it's value is not passed in the command-line, an exception will be thrown.", "md_docs_2tutorial.html#autotoc_md3-required---if-this-option-is-set-for-an-argument-and-its-value-is-not-passed-in-the-command-line-an-exception-will-be-thrown", null ],
          [ "4. <tt>suppress_arg_checks</tt> - Using a suppressing argument results in suppressing requirement checks for other arguments.", "md_docs_2tutorial.html#autotoc_md4-suppress_arg_checks---using-a-suppressing-argument-results-in-suppressing-requirement-checks-for-other-arguments", null ],
          [ "5. <tt>nargs</tt> - Sets the allowed number of values to be parsed for an argument.", "md_docs_2tutorial.html#autotoc_md5-nargs---sets-the-allowed-number-of-values-to-be-parsed-for-an-argument", null ],
          [ "6. <tt>greedy</tt> - If this option is set, the argument will consume ALL command-line values until it's upper nargs bound is reached.", "md_docs_2tutorial.html#autotoc_md6-greedy---if-this-option-is-set-the-argument-will-consume-all-command-line-values-until-its-upper-nargs-bound-is-reached", null ],
          [ "7. <tt>choices</tt> - A list of valid argument values.", "md_docs_2tutorial.html#autotoc_md7-choices---a-list-of-valid-argument-values", null ],
          [ "8. value actions - Functions that are called after parsing an argument's value.", "md_docs_2tutorial.html#autotoc_md8-value-actions---functions-that-are-called-after-parsing-an-arguments-value", null ],
          [ "9. <tt>default_values</tt> - A list of values which will be used if no values for an argument have been parsed", "md_docs_2tutorial.html#autotoc_md9-default_values---a-list-of-values-which-will-be-used-if-no-values-for-an-argument-have-been-parsed", null ]
        ] ],
        [ "Parameters Specific for Optional Arguments", "md_docs_2tutorial.html#parameters-specific-for-optional-arguments", [
          [ "1. on-flag actions - Functions that are called immediately after parsing an argument's flag.", "md_docs_2tutorial.html#autotoc_md1-on-flag-actions---functions-that-are-called-immediately-after-parsing-an-arguments-flag", null ],
          [ "2. <tt>implicit_values</tt> - A list of values which will be set for an argument if only its flag but no values are parsed from the command-line.", "md_docs_2tutorial.html#autotoc_md2-implicit_values---a-list-of-values-which-will-be-set-for-an-argument-if-only-its-flag-but-no-values-are-parsed-from-the-command-line", null ]
        ] ]
      ] ],
      [ "Predefined Parameter Values", "md_docs_2tutorial.html#predefined-parameter-values", [
        [ "Actions", "md_docs_2tutorial.html#actions", null ]
      ] ],
      [ "Default Arguments", "md_docs_2tutorial.html#default-arguments", null ],
      [ "Argument Groups", "md_docs_2tutorial.html#argument-groups", [
        [ "Creating New Groups", "md_docs_2tutorial.html#creating-new-groups", null ],
        [ "Adding Arguments to Groups", "md_docs_2tutorial.html#adding-arguments-to-groups", null ],
        [ "Group Attributes", "md_docs_2tutorial.html#group-attributes", null ],
        [ "Complete Example", "md_docs_2tutorial.html#complete-example", null ],
        [ "Suppressing Argument Group Checks", "md_docs_2tutorial.html#suppressing-argument-group-checks", null ]
      ] ],
      [ "Parsing Arguments", "md_docs_2tutorial.html#parsing-arguments", [
        [ "Basic Argument Parsing Rules", "md_docs_2tutorial.html#basic-argument-parsing-rules", [
          [ "1. Optional arguments are parsed only with a flag", "md_docs_2tutorial.html#autotoc_md1-optional-arguments-are-parsed-only-with-a-flag", null ],
          [ "2. Positional arguments are parsed in the order of definition", "md_docs_2tutorial.html#autotoc_md2-positional-arguments-are-parsed-in-the-order-of-definition", null ],
          [ "3. Positional arguments consume free values", "md_docs_2tutorial.html#autotoc_md3-positional-arguments-consume-free-values", null ],
          [ "4. Unknown Argument Flag Handling", "md_docs_2tutorial.html#autotoc_md4-unknown-argument-flag-handling", null ]
        ] ],
        [ "Compound Arguments", "md_docs_2tutorial.html#compound-arguments", null ],
        [ "Parsing Known Arguments", "md_docs_2tutorial.html#parsing-known-arguments", null ]
      ] ],
      [ "Retrieving Argument Values", "md_docs_2tutorial.html#retrieving-argument-values", null ],
      [ "Subparsers", "md_docs_2tutorial.html#subparsers", [
        [ "Creating Subparsers", "md_docs_2tutorial.html#creating-subparsers", null ],
        [ "Using Multiple Subparsers", "md_docs_2tutorial.html#using-multiple-subparsers", null ],
        [ "Parsing Arguments with Subparsers", "md_docs_2tutorial.html#parsing-arguments-with-subparsers", null ],
        [ "Tracking Parser State", "md_docs_2tutorial.html#tracking-parser-state", [
          [ "Example: Inspecting Parsing States", "md_docs_2tutorial.html#example-inspecting-parsing-states", null ]
        ] ]
      ] ],
      [ "Examples", "md_docs_2tutorial.html#examples", null ],
      [ "Common Utility", "md_docs_2tutorial.html#common-utility", null ]
    ] ],
    [ "Todo List", "todo.html", null ],
    [ "Topics", "topics.html", "topics" ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ],
        [ "Enumerations", "namespacemembers_enum.html", null ]
      ] ]
    ] ],
    [ "Concepts", "concepts.html", "concepts" ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ],
    [ "Examples", "examples.html", "examples" ]
  ] ]
];

var NAVTREEINDEX =
[
"_2home_2runner_2work_2cpp-ap_2cpp-ap_2include_2ap_2argument_parser_8hpp-example.html",
"classap_1_1argument__parser.html#a13b07a0743939b405c41a13cb90b9ede",
"classap_1_1detail_1_1help__builder.html#a8188afef1669fcb0c9981f78e25dc2ac",
"namespacemembers_enum.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';