# Repo Template

This library contains general configuration files and functions related to the RPi Pico W's bluetooth functionality. The library is intended to be used as a submodule in other projects.

## 🛠️ Setup

### 1. Add the Repository as a Submodule
```bash
git submodule add https://github.com/Danielaca18/pico-bt.git
```


### 3. Include the Library in Your Project
```cmake
add_subdirectory(pico-bt)
target_link_libraries(your_project pico-btlib)
```

## 💼 License
This project is licensed under the [MIT License](LICENSE), which is a permissive open-source license that allows you to use, modify, and distribute the code for both commercial and non-commercial purposes. You can find the full text of the license in the [LICENSE](LICENSE) file.