#!/bin/bash

OPERATING_SYSTEM="Unknown" # Possible values: Unknown, Linux, Windows

detectOsAndSetup() {
  local OS i

  OS="$(uname -s)"

  for i in cygwin mingw; do
    echo "$OS" | grep -i "$i" &> /dev/null
    if (( $? == 0 )); then
      OPERATING_SYSTEM="Windows"
      msg1 "Detected Operating System is '$OPERATING_SYSTEM'"
      setupWindows
      return
    fi
  done

  for i in linux; do
    echo "$OS" | grep -i "$i" &> /dev/null
    if (( $? == 0 )); then
      OPERATING_SYSTEM="Linux"
      msg1 "Detected Operating System is '$OPERATING_SYSTEM'"
      setupLinux
      return
    fi
  done
}


setupLinux() {
  GIT_EXEC="$(which git)" &>/dev/null

  [ -z "$GIT_EXEC" ] && warning "Unable to find GIT"
}



# WINDOWS

findGit() {
  local GIT_PATHS=(
    "/cygdrive/c/Program Files (x86)/Git/bin"
    "/cygdrive/c/Program Files/Git/bin"
  )

  local i GITP

  for i in "${GIT_PATHS[@]}"; do
    [ ! -f "$i/git.exe" ] && continue

    GIT_EXEC="$i/git.exe"
    GITP="$i"
    break
  done

  if [ -z "$GIT_EXEC" ]; then
    warning "Unable to find GIT"
  else
    GIT_V="$("$GITP/sh" -l -c exit | grep version | awk '{print $5}' | tr -d ')')"

    found "GIT version $GIT_V in '$GIT_EXEC'"
  fi
}

findCMake() {
  local CMAKE_EXES=(
    "/cygdrive/c/Program Files (x86)/CMake/bin/cmake.exe"
    "/cygdrive/c/Program Files/CMake/bin/cmake.exe"
  )

  local i

  for i in "${CMAKE_EXES[@]}"; do
    [ ! -f "$i" ] && continue

    CMAKE_EXEC="$i"
    break
  done

  if [ -z "$CMAKE_EXEC" ]; then
    warning "Unable to find CMake"
  else
    CMAKE_V="$("$CMAKE_EXEC" --version | grep version | awk '{print $3}')"

    found "CMake version $CMAKE_V in '$CMAKE_EXEC'"
  fi
}

setupWindows() {
  findGit
  findCMake

  local URL VERSION


  # Check for updates -- GIT

  URL="$(curl -s https://api.github.com/repos/msysgit/msysgit/releases \
    | grep browser_download_url | grep 'Git-[0-9]\+\.[0-9]\+\.[0-9]\+\-preview[0-9]\+\.exe' \
    | head -n 1 | cut -d '"' -f 4)"

  if [ -z "$URL" ]; then
    warning "Unable to get GIT version information"
    VERSION="ERROR"
  else
    VERSION="$(basename "$URL")"
    VERSION="${VERSION//.exe/}"
    VERSION="${VERSION/Git-/}"
  fi

  if [[ "$VERSION" == "$GIT_V" ]]; then
    msg2 "Your current GIT Version is up-to-date"
  elif [[ "$VERSION" != "ERROR" ]]; then
    if (( WIN_UPDATE == 1 )); then
      msg2 "New GIT version $VERSION found."
      ask "Update GIT" "Y" DO_UPDATE
      case "$DO_UPDATE" in
        Y|YES|yes|Yes) updateGit "$URL";;
        *)             msg2 "Do not update GIT" ;;
      esac
    else
      msg2 "New GIT version $VERSION found. Use option '-u' to update"
    fi
  fi

  # Check for updates -- CMAKE

  VERSION="$(curl -s https://api.github.com/repos/Kitware/CMake/tags | grep "name" | head -n 1 | cut -d '"' -f 4)"

  if [ -z "$VERSION" ]; then
    warning "Unable to get CMake version information"
    URL="ERROR"
  else
    # Bsp: http://www.cmake.org/files/v3.2/cmake-3.2.2-win32-x86.exe
    URL="http://www.cmake.org/files/${VERSION:0:4}/cmake-${VERSION:1}-win32-x86.exe"
  fi

  if [[ "${VERSION:1}" == "$CMAKE_V" ]]; then
    msg2 "Your current CMake Version is up-to-date"
  elif [[ "$VERSION" != "ERROR" ]]; then
    if (( WIN_UPDATE == 1 )); then
      msg2 "New CMake version $VERSION found."
      ask "Update CMake" "Y" DO_UPDATE
      case "$DO_UPDATE" in
        Y|YES|yes|Yes) updateCMake "$URL";;
        *)             msg2 "Do not update CMake" ;;
      esac
    else
      msg2 "New CMake version $VERSION found. Use option '-u' to update"
    fi
  fi

}

updateGit() {
  [ ! -d "$WINDOWS_SETUP_DIR" ] && mkdir "$WINDOWS_SETUP_DIR"

  cd "$WINDOWS_SETUP_DIR"
  local SETUPEXE="gitBash.exe"

  [ -e "$SETUPEXE" ] && rm "$SETUPEXE"

  wget "$1" -O "$SETUPEXE"
  chmod +x "$SETUPEXE"

  if [ ! -f "$SETUPEXE" ]; then
    error "Failed to download GIT"
    return
  fi

  msg1 "Finished Downloading GIT"
  msg2 "Starting the installer."
  echo ""
  msg3 "======================================="
  msg3 "= PLEASE USE THE DEFAULT INSTALL PATH ="
  msg3 "======================================="

  "./$SETUPEXE"

  msg2 "INSTALLER DONE"
  findGit

  cd ..
}

updateCMake() {
  [ ! -d "$WINDOWS_SETUP_DIR" ] && mkdir "$WINDOWS_SETUP_DIR"

  cd "$WINDOWS_SETUP_DIR"
  local SETUPEXE="cmake.exe"

  [ -e "$SETUPEXE" ] && rm "$SETUPEXE"

  wget "$1" -O "$SETUPEXE"
  chmod +x "$SETUPEXE"

  if [ ! -f "$SETUPEXE" ]; then
    error "Failed to download CMake"
    return
  fi

  msg1 "Finished Downloading CMake"
  msg2 "Starting the installer."
  echo ""
  msg3 "======================================="
  msg3 "= PLEASE USE THE DEFAULT INSTALL PATH ="
  msg3 "======================================="

  cygstart -w "./$SETUPEXE"

  msg2 "INSTALLER DONE"
  findCMake

  cd ..
}
