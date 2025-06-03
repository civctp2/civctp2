# Contributing

## Commits

Please try to comply with following guidelines:
- [How to Write a Git Commit Message](https://chris.beams.io/posts/git-commit/)
- or TL;DR version: https://github.com/erlang/otp/wiki/writing-good-commit-messages

## Pull Requests

If you want to add your changes to the repository, please create a pull request. You can already create a pull request, when you are still working on it. In this case, start the title of your pull request with WIP (Work In progress) and remove the WIP lable when you think your done. This way, people know that there is somebody working on that issue. And they can also give you some tips. However, we work on this project in our free time and people might only be available on the weekend. Therefore, when a pull requests reaches non WIP status, the maintainer will give at least 2 weeks time before merging, so that everybody has a hance to look it. This time can be shortened before an upcoming release, to incooperate last minute bug fixes and readme updates.

## Naming style

Names of functions, structs, classes, and variables are camel cased. Variables start with a small letter. Member variables are prefixed with m\_, static member variables are prefixed with s\_, and global variables are prefixed with g\_, local variables have no prefix. Enums are all caps. This is inherited from Activion for the main project and the other CTP2 related projects.

## Tabs and Spaces

Most employees at Activision used tabs for indention and a tab was 4 space wide. And so, most of the code is written in this way in the CTP2 project, the map plugins and the Anet library. Code from 3rd party libraries and automatically generated files may not adhere to this. Please, set up your editor accordingly and enable display of white space. For alignment use spaces. In case you decide to reformat source files do not unalign things that have been aligned. If you clean up white psace on large scale put these changes into white space only commits. Even so white space changes for tabs and spaces can be ignored in diff-viewer this is unfortunately not true for new lines. So do not mix large scale white space canges with functional changes, those are then hard to spot.

## AI Testing

To test the AI, set up a new game and save it before moving any unit or doing anything else with that. Then open the chat window by using the apostrophe key (') and enter `/attach 1` if the player on screen is player 1. Then enter `/rnd 200` and the AI will play 200 turns while you are watching. Important is that the show AI does not show any significant changes if you did not intent that. If you are using an old save game from a previous version type also into the chat window `/reloadslic` to reload slic. There seem to be some incompatibilities in slic from older versions, but that fixes that. The chat window has more commands, which you can find in ChatWindow::CheckForEasterEggs in `./ctp2_code/ui/interface/ChatBox.cpp`.

## Leaks

The debug version comes with a leak reporter. When CTP2 shutsdown, it reports the leaks to the files CTP_LEAKS_99999.TXT and CTP_LEAKS_ALT_99999.TXT in the directory of the executable. The number in the file name is supposed to be some turn count, but it is always 99999. The file CTP_LEAKS_99999.TXT is the version from the original reporter. Displays the stacks, in which the leak occured from top to bottom. The other file CTP_LEAKS_ALT_99999.TXT displays the stacks from bottom to top. This way, all shared parts f the stack are alingned and the places where the stacks branch are good places to look for the leaks. If the is no leak to report, the files will say None. This is how it should be.

## Logging

The debug version writes log files to `\ctp2_code\ctp\logs` (Windows 32 bit and Linux version) or `\ctp2_code\ctp\x64\logs` (Windows 64 bit version). Visual Studio has a Logging conviguration that generates version with logging. It is optimized like Final and identical to that except for logging. The logs allow you to understand what the AI is doing or the state of diplomacy. There are various flags that determine what is being logged. They are set in main_InitializeLogs in `./ctp2_code/ctp/civ3_main.cpp`, the flags are defined in ´./ctp2_code/ctp/ctp2_utils/c3debug.h`.

## Graphics

New graphics should adhere to the style of the original CTP2 team. For instance, the items in the Great Library have a drop shaddow and their edges are blured to blend in the background. For GIMP here is a [tutorial for dropping shaddows](https://thegimptutorials.com/how-to-add-drop-shadow/) and [one for blurring](https://thegimptutorials.com/how-to-blur-background/) actually you want the smudge tool there.

## Working with Visual Studio

Visual Studio is a powerful Integrated Development Environment. If you are not familiar with it you are expected to familiarize yourself with it. Here are some tips: To understand the code you have to read the code and figure what does what, at least for the pieces you are interested in. For that you must look up function, you can either use the integrated search function. It has three option, search the next occurence, the last occurence and all occurences. All occurences is very useful, as it gives you a list with soe context, so that you can decide what interests you. You can also search in the whole solution, in the current project, in the current file, and in the current selection. Most likely the current project is the most useful to you, as FFmpeg code is not really interesting if you want to find something that you know is in the ctp2 project.

From the right click menu you can go directly to the declarations and definitions of functions and variables. The same goes for include files, even for those of the Windows SDK and the standard library. There is also a show the call stack option. You can also build, link, and CTP2 from inside the Visual Studio if you have setup your working copy, directly. For that just press F5 to build, link, and run CTP2 on the debugger and Ctrl F5 without the debugger.

## Miscelanious

[New issue](https://github.com/civctp2/civctp2/issues/new)<br>
[Issues](https://github.com/civctp2/civctp2/issues)<br>
[Fork](https://github.com/civctp2/civctp2/fork)<br>
[Email](mailto:civctp2@gmail.com)
