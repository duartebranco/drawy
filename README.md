> [!NOTE]
> Drawy has moved to KDE Invent. This repository is kept only to track and resolve existing issues. Please open all new issues and pull requests on KDE Invent instead.
>   
> https://invent.kde.org/prayag/drawy
>
> Please be aware that Drawy does **NOT** currently offer official packages in any public repository. However, official nightly packages are available on [KDE Invent](https://invent.kde.org/graphics/drawy/-/jobs?kind=BUILD)  

  
<p align="center">
  <img src="assets/logo-256.png" width=100/>
  <h1 align="center">Drawy</h1>
  <p align="center">Your handy, infinite brainstorming tool!</center>
</p>

<p align="center">
<a href="https://github.com/prayag2/drawy/stargazers"><img alt="GitHub Stars" src="https://img.shields.io/github/stars/prayag2/drawy?style=for-the-badge&logo=github&labelColor=%23202025&color=%23205CC0"></a>
<a href="https://github.com/prayag2/drawy/network"><img alt="GitHub Forks" src="https://img.shields.io/github/forks/prayag2/drawy?style=for-the-badge&logo=github&labelColor=%23202025&color=%23205CC0"></a>
<a href="https://github.com/prayag2/drawy/releases"><img alt="GitHub Release" src="https://img.shields.io/github/v/release/prayag2/drawy?include_prereleases&style=for-the-badge&labelColor=%23202025&color=%23205CC0"></a>
</p>

<img src="./assets/screenshot.png" style="width: 100%"/>

Drawy is a work-in-progress infinite whiteboard tool written in Qt/C++, which aims to be a native-desktop alternative to the amazing web-based Excalidraw.  

# Installation  
<a href="https://github.com/Prayag2/drawy/releases/download/1.0.0-alpha/Drawy-47b7552-x86_64.AppImage">
<img alt="Static Badge" src="https://img.shields.io/badge/Download-Linux-F7B601?style=for-the-badge&labelColor=%23202025">
</a>

<a href="https://github.com/Prayag2/drawy/releases/download/1.0.0-alpha/drawy-windows-x86_64.zip">
<img alt="Static Badge" src="https://img.shields.io/badge/Download-Windows-007CF7?style=for-the-badge&labelColor=%23202025">
</a>

## Compiling from Source
- Install `cmake` and `g++`
- Install Qt 6.9 or above from [here](https://www.qt.io/download-qt-installer-oss) or using [aqtinstall](https://github.com/miurahr/aqtinstall)
- Clone this repository: `git clone https://github.com/prayag2/drawy && cd drawy`
- Setup cmake: `cmake -B build -S . -DCMAKE_BUILD_TYPE=Release`
- Compile: `cmake --build build --config Release`
- Run: `./build/drawy`

# Keyboard Shortcuts
Future releases will allow you to change the keyboard shortcuts. For now they are hardcoded. Here's a list of all available keyboard shortcuts:
| Key Combination                                                             | Description       |
|:---------------------------------------------------------------------------:|:-----------------:|
| <kbd>Ctrl</kbd> + <kbd>Z</kbd>                                              | Undo              |
| <kbd>Ctrl</kbd> + <kbd>Y</kbd>, <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>Z</kbd> | Redo              |
| <kbd>Ctrl</kbd> + <kbd>+</kbd>                                                | Zoom In           |
| <kbd>Ctrl</kbd> + <kbd>-</kbd>                                                | Zoom Out          |
| <kbd>Ctrl</kbd> + <kbd>G</kbd>                                                | Group selection   |
| <kbd>Ctrl</kbd> + <kbd>Shift</kbd> + <kbd>G</kbd>                               | Ungroup selection |
| <kbd>P</kbd>, <kbd>B</kbd>                                                  | Freeform Tool     |
| <kbd>E</kbd>                                                                | Eraser Tool       |
| <kbd>S</kbd>                                                                | Selection Tool    |
| <kbd>R</kbd>                                                                | Rectangle Tool    |
| <kbd>O</kbd>                                                                | Ellipse Tool      |
| <kbd>L</kbd>                                                                | Line Tool         |
| <kbd>A</kbd>                                                                | Arrow Tool        |
| <kbd>M</kbd>                                                                | Move Tool         |
| <kbd>T</kbd>                                                                | Text Tool         |
| <kbd>Ctrl+A</kbd>                                                           | Select All        |
| <kbd>Delete</kbd>                                                           | Delete selection  |
| <kbd>Ctrl+S</kbd>                                                           | Save              |
| <kbd>Ctrl+O</kbd>                                                           | Open File         |

# Contributing
Please open merge requests on KDE Invent: https://invent.kde.org/graphics/drawy

# License
This project uses the GNU General Public License V3.
