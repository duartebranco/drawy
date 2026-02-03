> [!NOTE]
> Drawy has moved to KDE Invent. This repository is kept only to track and resolve existing issues. Please open all new issues and pull requests on KDE Invent instead.
>
> https://invent.kde.org/prayag/drawy
>
> Please be aware that Drawy does **NOT** currently offer official packages in any public repository. However, official nightly packages are available on [KDE Invent](https://invent.kde.org/graphics/drawy/-/jobs?kind=BUILD)

Drawy is a work-in-progress infinite whiteboard tool written in Qt/C++, which aims to be a native-desktop alternative to the amazing web-based Excalidraw.

# MY FORK

This fork has some new features that i found indispensable and i wanted to have right now, such as:

- Settings tab;
- Remember theme mode preference;
- Open last edited file;
- Save same file when its being edited;
- Possible more in the future ...

I did this _fast_ and _just_ for me.

I'm sure someone will implement this features sooner or later and with more attention to detail than me.

But given the KDE Invent migration, I won't make any PR, so...

If you want these features, ask them and, in the meantime, you can use this fork while you wait.

# Compiling from Source
- Install `cmake` and `g++`
- Install Qt 6.9 or above from [here](https://www.qt.io/download-qt-installer-oss) or using [aqtinstall](https://github.com/miurahr/aqtinstall)
- Clone this repository: `git clone https://github.com/duartebranco/drawy && cd drawy`
- Setup cmake: `cmake -B build -S . -DCMAKE_BUILD_TYPE=Release`
- Compile: `cmake --build build --config Release`
- Run: `./build/drawy`
