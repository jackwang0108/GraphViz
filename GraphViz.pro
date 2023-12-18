QT += widgets

# Make sure we do not accidentally #include files placed in 'res'
CONFIG += no_include_pwd
CONFIG += console
CONFIG -= app_bundle

SOURCES += $$PWD/src/*.cpp
SOURCES += $$PWD/src/*.h
HEADERS += $$PWD/src/*.h

# set up flags for the compiler and Stanford C++ libraries
QMAKE_CXXFLAGS += -std=c++20 \
    -g \
    -Wall \
    -Wextra \
    -Wreturn-type \
    -Werror=return-type \
    -Wunreachable-code \

# Copies the given files to the destination directory
# The rest of this file defines how to copy the resources folder
defineTest(copyToDestdir) {
    files = $$1

    for(FILE, files) {
        DDIR = $$OUT_PWD

        # Replace slashes in paths with backslashes for Windows
        win32:FILE ~= s,/,\\,g
        win32:DDIR ~= s,/,\\,g

        !win32 {
            copyResources.commands += cp -r '"'$$FILE'"' '"'$$DDIR'"' $$escape_expand(\\n\\t)
        }
        win32 {
            copyResources.commands += xcopy '"'$$FILE'"' '"'$$DDIR'"' /e /y $$escape_expand(\\n\\t)
        }
    }
    export(copyResources.commands)
}

!win32 {
    copyToDestdir($$files($$PWD/res/*))
}
win32 {
    copyToDestdir($$PWD/res)
}

copyResources.input = $$files($$PWD/res/*)
OTHER_FILES = $$files(res/*)
QMAKE_EXTRA_TARGETS += copyResources
POST_TARGETDEPS += copyResources

macx {
    cache()
    QMAKE_MAC_SDK = macosx
}

# Get the list of files in the 'res' directory
RES_FILES = $$files(res/*)

# Extract the base names of the files
CLEAN_FILES = $$basename(RES_FILES)

# Add the files to the DISTCLEAN variable
DISTCLEAN += $$CLEAN_FILES

# Define a new target that depends on the 'first' target
distclean.target = distclean
distclean.depends = first
# distclean.commands = for(file, CLEAN_FILES) {\
#                         distclean.commands += -$(DEL_FILE) $$file $$escape_expand(\\n\\t)\
#                     }

# Add the new target to the QMAKE_EXTRA_TARGETS variable
QMAKE_EXTRA_TARGETS += distclean
