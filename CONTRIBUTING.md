# Contributing

## Commits

Please try to comply with following guidelines:
- [How to Write a Git Commit Message](https://chris.beams.io/posts/git-commit/)
- or TL;DR version: https://github.com/erlang/otp/wiki/writing-good-commit-messages

## Pull Requests

If you want to add your changes to the repository, please create a pull request. You can already create a pull request, when you are still working on it. In this case, start the title of your pull request with WIP (Work In progress) and remove the WIP lable when you think your done. This way, people know that there is somebody working on that issue. And they can also give you some tips. However, we work on this project in our free time and people might only be available on the weekend. Therefore, when a pull requests reaches non WIP status, the maintainer will give at least 2 weeks time before merging, so that everybody has a hance to look it. This time can be shortened before an upcoming release, to incooperate last minute bug fixes and readme updates.

## Naming style

Names of functions, structs, classes, and variables are camel cased. Variables start with a small letter. Member variables are prefixed with m_, static member variables are prefixed with s_, and global variables are prefixed with g_, local variables have no prefix. Enums are all caps. This is inherited from Activion for the main project and the other CTP2 related projects.

## Tabs and Spaces

Most employees at Activision used tabs for indention and a tab was 4 space wide. And so, most of the code is written in this way in the CTP2 project, the map plugins and Anet library. Code from 3rd party libraries and automatically generated files may not adhere to this. Please, set up your editor accordingly and enable display of white space.

## AI Testing

To test the AI, set up a new game and save it before moving any unit or doing anything else with that. Then open the chat window by using the apostrophe jey (') and enter /attach 1 if the player on screen is player 1. Then enter /rnd 200 and the AI will play 200 while you are watching. You can have the AI also have playing more then 200 turns. Important is that the show AI does not show any significant changes if you did not intent that.

## Leaks

The debug version comes with a leak reporter. When CTP2 shutsdown, it reports the leaks to the files CTP_LEAKS_99999.TXT and CTP_LEAKS_ALT_99999.TXT in the directory of the executable. The number in the file name is supposed to be some turn count, but it is always 99999. The file CTP_LEAKS_99999.TXT is the version from the original reporter. Displays the stacks, in which the leak occured from top to bottom. The other file CTP_LEAKS_ALT_99999.TXT displays the stacks from bottom to top. This way, all shared parts f the stack are alingned and the places where the stacks branch are good places to look for the leaks. If the is no leak to report, the files will say None. This is how it should be.

## Miscelanious

[issue]: https://github.com/civctp2/civctp2/issues/new
[issues]: https://github.com/civctp2/civctp2/issues
[fork]: https://github.com/civctp2/civctp2/fork
[email]: mailto:civctp2@gmail.com
