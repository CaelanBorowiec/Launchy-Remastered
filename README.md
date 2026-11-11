# Launchy Remastered

Launchy is a free keystroke launcher. Hit Alt+Space, type a few letters, and open programs, files, folders, or bookmarks without digging through the Start menu.

Launchy Remastered keeps Launchy alive on modern Windows. The original work from Josh Karlin and Simon Capewell still does the job, but Windows 10 and 11 brought display scaling, dark mode, and newer tooling that the old builds never knew about. This is a continuation of that Qt Launchy line, brought up to date.

## Quick start

Install it, then press **Alt+Space**. Type what you want and hit Enter. Right-click Launchy (outside the text boxes) for Options, skins, and plugins.

For the full manual (plugins, options, shortcuts, and command line switches), see the [original Launchy readme](Launchy11/Readme.md).

## What's new

- Built on Qt 6, the toolkit behind Launchy's window and options. The old Qt 4 it used is long out of support, so this keeps Launchy building and running on current Windows.
- Per-monitor display scaling and better window placement on high-DPI setups
- Follows Windows dark mode (Fusion style while the system theme is dark)
- About page and update checks pointed at this GitHub project
- Calcy plugin updated for the Qt 6 build

## Where this came from

Launchy was created by [Josh Karlin](https://www.launchy.net/). [Simon Capewell](https://sourceforge.net/projects/launchy/) later joined as a developer, and a lot of the 2.5 era work is his. The official homepage is still [launchy.net](https://www.launchy.net/).

This repo started from the classic Qt Launchy sources (the 2.6 beta line from SourceForge) and carries that code forward for Windows 11 and current tooling. Launchy Remastered is maintained by Caelan Borowiec.

## License

GNU GPL. See [LICENSE](LICENSE) and [Launchy11/LICENSE.txt](Launchy11/LICENSE.txt).
