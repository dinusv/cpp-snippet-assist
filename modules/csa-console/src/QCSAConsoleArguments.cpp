#include "QCSAConsoleArguments.hpp"
#include "QCSAConsole.hpp"

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>

QCSAConsoleArguments::QCSAConsoleArguments(const QCoreApplication &application, const QString &applicationDescription)
    : m_commandLineParser(new QCommandLineParser)
    , m_cursorOffset(-1)
    , m_cursorLine(-1)
    , m_cursorColumn(-1)
    , m_logLevel(csa::QCSAConsole::getLogLevel())
    , m_executeAndQuitFlag(false){

    m_headerSearchPatterns << "*.c" << "*.C" << "*.cxx" << "*.cpp" << "*.c++" << "*.cc" << "*.cp";
    m_sourceSearchPatterns << "*.h" << "*.H" << "*.hxx" << "*.hpp" << "*.h++" << "*.hh" << "*.hp";

    initialize(application, applicationDescription);
}

QCSAConsoleArguments::~QCSAConsoleArguments(){
    delete m_commandLineParser;
}

void QCSAConsoleArguments::initialize(const QCoreApplication &app, const QString &applicationDescription){

    // Setup parsing
    // -------------

    m_commandLineParser->setApplicationDescription(applicationDescription);
    m_commandLineParser->addHelpOption();
    m_commandLineParser->addVersionOption();
    m_commandLineParser->addPositionalArgument(
                "paths", QCoreApplication::translate("main", "Files and directories to parse."));

    QCommandLineOption cursorOffset("c",
        QCoreApplication::translate("main", "User cursor offset within the file."),
        QCoreApplication::translate("main", "offset")
    );
    m_commandLineParser->addOption(cursorOffset);

    QCommandLineOption cursorLineColumnOffset("lc",
        QCoreApplication::translate("main", "Line:Column of the user cursor within the file."),
        QCoreApplication::translate("main", "line:column")
    );
    m_commandLineParser->addOption(cursorLineColumnOffset);

    QCommandLineOption selectedFunction(QStringList() << "e" << "function",
        QCoreApplication::translate("main", "Selected javascript function from the loaded plugins."),
        QCoreApplication::translate("main", "function")
    );
    m_commandLineParser->addOption(selectedFunction);

    QCommandLineOption projectDir("dir",
        QCoreApplication::translate("main", "Project base directory.")
    );
    m_commandLineParser->addOption(projectDir);

    QCommandLineOption logLevel("loglevel",
        QCoreApplication::translate("main", "Log level of the application. (0 - General, 1 - Warning, 2 - Debug, "
                                            "3 - Info_Level_1, 4 - Info_Level_2"),
        QCoreApplication::translate("main", "0-4")
    );
    m_commandLineParser->addOption(logLevel);

    QCommandLineOption executeAndQuit("f",
        QCoreApplication::translate("main", "Execute function and quit.")
    );
    m_commandLineParser->addOption(executeAndQuit);

    // Process arguments
    // -----------------

    m_commandLineParser->process(app);

    // Get Positional Arguments
    // ------------------------

    QStringList positionalArgs = m_commandLineParser->positionalArguments();
    if ( positionalArgs.size() == 0 )
        m_commandLineParser->showHelp(1);

    for ( QStringList::iterator it = positionalArgs.begin(); it != positionalArgs.end(); ++it ){
        QString& val = *it;

        if ( !QFileInfo::exists(val) ){
            m_fileErrors.append("Error: Path does not exist: " + val);
        } else {
            m_files.append(*it);
        }
    }

    // Get Option Values
    // -----------------

    m_functionSet = m_commandLineParser->isSet(selectedFunction);
    if ( m_functionSet )
        m_function = m_commandLineParser->value(selectedFunction);

    m_cursorOffset = m_commandLineParser->isSet(cursorOffset) ? m_commandLineParser->value(cursorOffset).toInt() : -1;
    if ( m_cursorOffset == -1 ){
        bool isLineColumnSet = m_commandLineParser->isSet(cursorLineColumnOffset);
        if ( isLineColumnSet ){
            QStringList lineColSplit = m_commandLineParser->value(cursorLineColumnOffset).split(":");
            if ( lineColSplit.size() != 2 )
                m_commandLineParser->showHelp(2);

            bool lineConvertOk, colConvertOk;
            m_cursorLine   = lineColSplit[0].toInt(&lineConvertOk);
            m_cursorColumn = lineColSplit[1].toInt(&colConvertOk);
            if ( !lineConvertOk || !colConvertOk )
                m_commandLineParser->showHelp(3);

        } else {
            m_cursorColumn = -1;
            m_cursorLine   = -1;
        }
    }

    m_logLevel = m_commandLineParser->isSet(logLevel) ? m_commandLineParser->value(logLevel).toInt() : m_logLevel;

    m_projectDir          = m_commandLineParser->isSet(projectDir) ? m_commandLineParser->value(projectDir) : "";
    m_executeAndQuitFlag  = m_commandLineParser->isSet(executeAndQuit);
}
