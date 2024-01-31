# Code Quality Report

During the course of this project all members of the team have contributed to producing code of high quality with respect to the characteristics of *readability*, *maintainability*, *performance*, *correctness* and *testability*. This has been achieved by employing a set of common *standards* with a clear *process* for design, implementation and review of our code. We have ensured a consistent and sensible *structure*, with approprite *formatting* and denomination of variables and functions. All code is in addition extensively and consistently *documented*.

## Standards
The [Apache-C](https://httpd.apache.org/dev/styleguide.html) code standard was chosen for its simplicity and clear documentation, and followed throughout the project. This was employed both consciously by group members when typing code, with help of IDEs, and with a final autoformatting of all code at the end of the project. A consistent method for error handling was agreed upon, with the use of assertions and the philosophy ”crash don’t trash” deemed appropriate for an  automated GC that would be handling user memory. A set of shared design documents were utilised  to ensure consistency between modules and a common understanding of their operations.

## Process
A clear process for writing, reviewing and implementing code has been employed, which has sought to continually improve its quality by getting as many ”eyes” on it as possible, as a means of peer review. The use of pair programming has meant that at least two group members have been responsible for each module, allowing each to review and improve the code of the other. Partners have additionally been switched during each sprint, allowing other team members to review and contribute to existing code. The testability of code was established by writing unit tests for each module, and its correctness by ensuring that these tests passed before pushing and merging to GitHub. All code was pushed to separate branches, and underwent both automated CI testing and a formal review with a checklist by another group member before being merged. See [Team Reflection](team_reflection.md) for further details.
Both individual modules and larger sections of the codebase have also been refactored to improve aspects of readability, remove ”bad smells” and ensure a better separation of concerns. Once all of the code had been merged, a final ”polishing” was also undertaken, removing all TODOs, FIXMEs, unnecessary comments, magic numbers, etc.

## Structure
The code is divided into clear, sensible modules, chosen so as to reduce coupling between them and increase cohesion within. Functions and variables have been given descriptive names that aid readability and seek to offer a ”narrative” feel to the code, while magic numbers, boolean values, etc. have been defined with descriptive macros. Public functions have been created within modules for accessing and manipulating their data structures, again considering the principle of separation of concerns, and allowing a degree of information hiding between individual modules. This, combined with an attempt to employ a good level of functional abstraction, where repeated code is extracted and each function implements - on the whole - one procedure, has hopefully contributed to the readability and maintainability of the code. Macros, type definitions, etc. are defined in a module’s respective header file where appropriate, or in the file `common.h` when used by several modules.

## Documentation
All public functions have been clearly documented in their module’s respective header file, and static functions documented inline where it was deemed that this would help with readability and maintainability. Additional explanatory comments were added to explain complex algorithms or calculations or where ”magic numbers” could not easily be abstracted to reasonably-named macros (certain bitshift operations, etc).

During the initial code-writing process, the focus has been upon readability and maintainability with  tests providing a check on testability and correctness. ”Quick” implementations with questionable performance characteristics were left as TODOs of FIXMEs which could – if necessary – be altered later once profiling had been carried out.
