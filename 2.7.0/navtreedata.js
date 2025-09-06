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
      [ "Building and testing", "md_docs_2dev__notes.html#building-and-testing", [
        [ "Build the testing executable", "md_docs_2dev__notes.html#build-the-testing-executable", null ],
        [ "Run the tests", "md_docs_2dev__notes.html#run-the-tests", null ]
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
      [ "Adding Arguments", "md_docs_2tutorial.html#adding-arguments", null ],
      [ "Argument Parameters", "md_docs_2tutorial.html#argument-parameters", [
        [ "Common Parameters", "md_docs_2tutorial.html#common-parameters", [
          [ "1. <tt>help</tt> - The argument's description which will be printed when printing the parser class instance.", "md_docs_2tutorial.html#autotoc_md1-help---the-arguments-description-which-will-be-printed-when-printing-the-parser-class-instance", null ],
          [ "2. <tt>hidden</tt> - If this option is set for an argument, then it will not be included in the program description.", "md_docs_2tutorial.html#autotoc_md2-hidden---if-this-option-is-set-for-an-argument-then-it-will-not-be-included-in-the-program-description", null ],
          [ "3. <tt>required</tt> - If this option is set for an argument and it's value is not passed in the command-line, an exception will be thrown.", "md_docs_2tutorial.html#autotoc_md3-required---if-this-option-is-set-for-an-argument-and-its-value-is-not-passed-in-the-command-line-an-exception-will-be-thrown", null ],
          [ "4. <tt>bypass_required</tt> - If this option is set for an argument, the <tt>required</tt> option for other arguments will be discarded if the bypassing argument is used in the command-line.", "md_docs_2tutorial.html#autotoc_md4-bypass_required---if-this-option-is-set-for-an-argument-the-required-option-for-other-arguments-will-be-discarded-if-the-bypassing-argument-is-used-in-the-command-line", null ],
          [ "5. <tt>default_value</tt> - The default value for an argument which will be used if no values for this argument are parsed", "md_docs_2tutorial.html#autotoc_md5-default_value---the-default-value-for-an-argument-which-will-be-used-if-no-values-for-this-argument-are-parsed", null ],
          [ "6. <tt>choices</tt> - A list of valid argument values.", "md_docs_2tutorial.html#autotoc_md6-choices---a-list-of-valid-argument-values", null ],
          [ "7. Value actions - Function performed after parsing an argument's value.", "md_docs_2tutorial.html#autotoc_md7-value-actions---function-performed-after-parsing-an-arguments-value", null ]
        ] ],
        [ "Parameters Specific for Optional Arguments", "md_docs_2tutorial.html#parameters-specific-for-optional-arguments", [
          [ "1. <tt>nargs</tt> - Sets the allowed number of values to be parsed for an argument. This can be set as a:", "md_docs_2tutorial.html#autotoc_md1-nargs---sets-the-allowed-number-of-values-to-be-parsed-for-an-argument-this-can-be-set-as-a", null ],
          [ "2. <tt>implicit_value</tt> - A value which will be set for an argument if only it's flag is parsed from the command-line but no values follow.", "md_docs_2tutorial.html#autotoc_md2-implicit_value---a-value-which-will-be-set-for-an-argument-if-only-its-flag-is-parsed-from-the-command-line-but-no-values-follow", null ],
          [ "4. On-flag actions - For optional arguments, apart from value actions, you can specify on-flag actions which are executed immediately after parsing an argument's flag.", "md_docs_2tutorial.html#autotoc_md4-on-flag-actions---for-optional-arguments-apart-from-value-actions-you-can-specify-on-flag-actions-which-are-executed-immediately-after-parsing-an-arguments-flag", null ]
        ] ]
      ] ],
      [ "Predefined Parameter Values", "md_docs_2tutorial.html#predefined-parameter-values", [
        [ "Actions", "md_docs_2tutorial.html#actions", null ]
      ] ],
      [ "Default Arguments", "md_docs_2tutorial.html#default-arguments", null ],
      [ "Parsing Arguments", "md_docs_2tutorial.html#parsing-arguments", [
        [ "Argument Parsing Rules", "md_docs_2tutorial.html#argument-parsing-rules", null ],
        [ "Compound Arguments", "md_docs_2tutorial.html#compound-arguments", null ],
        [ "Parsing Known Arguments", "md_docs_2tutorial.html#parsing-known-arguments", null ]
      ] ],
      [ "Retrieving Argument Values", "md_docs_2tutorial.html#retrieving-argument-values", null ],
      [ "Examples", "md_docs_2tutorial.html#examples", null ]
    ] ],
    [ "Todo List", "todo.html", null ],
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
        [ "Functions", "functions_func.html", null ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Enumerations", "functions_enum.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"annotated.html",
"classap_1_1argument__parser.html#a872eb42963ca9920282b6ed38bb792b6",
"functions_l.html",
"structap_1_1invalid__configuration_1_1positional.html"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';