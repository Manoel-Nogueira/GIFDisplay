# 🖼️ GIFDisplay

**GIFDisplay** was born from a simple frustration :\
I couldn't find a GIF display tool for Linux that worked the way I wanted.

So I thought:

> "Since I like C... why not build my own?"

And that's how this project started.

------------------------------------------------------------------------

## 🚀 Technologies Used

-   🧠 **C**
-   🖼️ **GTK 3**
-   🎵 **GStreamer**

------------------------------------------------------------------------

## ✨ Features

-   ✅ GIF display in its own window
-   ✅ Infinite loop playback
-   ✅ Scale control 
-   ✅ Background music playback in loop
-   ✅ Preview system
-   ✅ Simple to add new gifs and music

------------------------------------------------------------------------

## 📁 Project Structure

    GIFDisplay/
    │
    ├── main.c
    ├── makefile
    ├── style.css
    ├── assets/
    │   ├── fonts/
    │   └── images/
    ├── gifs/
    ├── musics/
    ├── README.md
    └── LICENSE

------------------------------------------------------------------------

## ➕ Adding Your Own GIFs and Music

You can easily customize the application by adding your own files.

### 🖼️ Adding New GIFs

>  Place your `.gif` files inside the `gifs/` folder.

------------------------------------------------------------------------

### 🎵 Adding New Music

>  Place your audio `.mp3` file inside the `musics/` folder.

------------------------------------------------------------------------

## ⚙️ Dependencies

You need the following installed:

-   GTK 3
-   GStreamer
-   pkg-config
-   GCC

### 🟢 Ubuntu / Debian

``` bash
sudo apt install build-essential libgtk-3-dev libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev libcairo2-dev libfontconfig1-dev
```

------------------------------------------------------------------------

### 🟣 Arch Linux

``` bash
sudo pacman -S base-devel gtk3 gstreamer gst-plugins-base cairo fontconfig pkgconf
```

------------------------------------------------------------------------

### 🔵 Fedora

``` bash
sudo dnf install @development-tools gtk3-devel gstreamer1-devel gstreamer1-plugins-base-devel cairo-devel fontconfig-devel pkgconf-pkg-config
```

------------------------------------------------------------------------

### 🟡 openSUSE

``` bash
sudo zypper install -t pattern devel_basis
sudo zypper install gtk3-devel gstreamer-devel gstreamer-plugins-base-devel cairo-devel fontconfig-devel pkg-config
```

------------------------------------------------------------------------


## 🔨 Build

``` bash
make all
```

------------------------------------------------------------------------

## ▶️ Run

``` bash
./GIFDisplay.out
```

------------------------------------------------------------------------

## 📌 Future Improvements

-   🔊 Music volume control
-   🖼️ GIF FPS control
-   🎛️ More refined UI

------------------------------------------------------------------------

## 📜 License

MIT License

------------------------------------------------------------------------

## 🧠 Why C?

Because:

-   I like understanding what happens under the hood
-   I want full control
-   GTK is powerful
-   And honestly... building UI in C has its own charm 😌

------------------------------------------------------------------------

## 🐧 Author

Developed by Manoel Nogueira Melo Filho
