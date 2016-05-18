# Copy tests to release path
# --------------------------

TEST_DATA_DEPLOY_FROM = $$PWD/core

win32:CONFIG(debug, debug|release): TEST_DATA_DEPLOY_TO = $$OUT_PWD/../build/core
else:win32:CONFIG(release, debug|release): TEST_DATA_DEPLOY_TO = $$OUT_PWD/../build/core
else:unix: TEST_DATA_DEPLOY_TO = $$OUT_PWD

win32:TEST_DATA_DEPLOY_TO ~= s,/,\\,g
win32:TEST_DATA_DEPLOY_FROM ~= s,/,\\,g

testdatacopy.commands = $(COPY_DIR) \"$$TEST_DATA_DEPLOY_FROM\" \"$$TEST_DATA_DEPLOY_TO\"
first.depends = $(first) testdatacopy
export(first.depends)
export(testdatacopy.commands)
QMAKE_EXTRA_TARGETS += first testdatacopy


# Enable javascript and console output
# ------------------------------------

QT     += qml quick script testlib
CONFIG += console testcase

## Configure Dependencies
## ----------------------

INCLUDEPATH += $$PWD/../../modules/csa-lib/astnodes
DEPENDPATH  += $$PWD/../../modules/csa-lib/astnodes
INCLUDEPATH += $$PWD/../../modules/csa-lib/codebase
DEPENDPATH  += $$PWD/../../modules/csa-lib/codebase
INCLUDEPATH += $$PWD/../../modules/csa-lib/script
DEPENDPATH  += $$PWD/../../modules/csa-lib/script
INCLUDEPATH += $$PWD/../../modules/csa-lib/testing
DEPENDPATH  += $$PWD/../../modules/csa-lib/testing

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../../modules/csa-lib/release/ -lcsa
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../../modules/csa-lib/debug/ -lcsa
else:unix: LIBS += -L$$OUT_PWD/../../modules/build/ -lcsa

include($$PWD/src/functionaltests.pri)
include($$PWD/../../3rdparty/libclang.pro)

TARGET   = csa-test-script
TEMPLATE = app

win32:CONFIG(release, debug|release): DESTDIR = $$OUT_PWD/../build
else:win32:CONFIG(debug, debug|release): DESTDIR = $$OUT_PWD/../build
else:unix: DESTDIR = $$OUT_PWD/../build

OTHER_FILES += \
    $$PWD/core/codebase-test.js

DISTFILES += \
    core/astfile-test.js \
    core/astnode-test.js \
    core/astmethod-test.js \
    core/nodecollection-test.js \
    core/token-test.js \
    core/config-test.js \
    core/codebase-test.in \
    core/codebase-test.insrc
