/****************************************************************************
**
** Copyright (C) 2014 Dinu SV.
** (contact: mail@dinusv.com)
** This file is part of C++ Snippet Assist application.
**
** GNU General Public License Usage
** 
** This file may be used under the terms of the GNU General Public License 
** version 3.0 as published by the Free Software Foundation and appearing 
** in the file LICENSE.GPL included in the packaging of this file.  Please 
** review the following information to ensure the GNU General Public License 
** version 3.0 requirements will be met: http://www.gnu.org/copyleft/gpl.html.
**
****************************************************************************/


#include "QCodeBase.hpp"
#include "QSyntaxTreeModel.hpp"
#include "QSourceLocation.hpp"
#include "QTokenClassifier.hpp"
#include "QASTFile.hpp"
#include "QASTNode.hpp"
#include "QASTVisitor.hpp"
#include <QMap>
#include <QHash>
#include <QDebug>

namespace csa{

using namespace ast;

QCodeBase::QCodeBase(
        const char* const* translationUnitArgs,
        int                translationUnitNumArgs,
        const QString&     file,
        QSyntaxTreeModel*   treeModel,
        QObject*           parent)

    : QObject(parent)
    , m_root(0)
    , m_current(0)
    , m_treeModel(treeModel)
    , m_classifier(0){

    CXIndex index = clang_createIndex(0, 0);
    CXTranslationUnit transUnit  = clang_parseTranslationUnit(
                index, file.toStdString().c_str(),
                translationUnitArgs,
                translationUnitNumArgs,
                0, 0,
                CXTranslationUnit_Incomplete | CXTranslationUnit_CXXChainedPCH);

    CXCursor startCursor  = clang_getTranslationUnitCursor(transUnit);
    CXFile   clangFile    = clang_getFile(transUnit, file.toStdString().c_str());

    m_classifier = new QTokenClassifier(transUnit, file.toStdString().c_str());

    m_root       = new QASTFile(
                        0,
                        file,
                        new QSourceLocation(
                            clang_getLocationForOffset(
                                transUnit,
                                clangFile,
                                QTokenClassifier::getFileSize(file.toStdString().c_str()))));
    m_files.append(m_root);
    m_current = m_root;

    QASTVisitor::createCSANodeTree(startCursor, m_root, m_classifier);

    if( m_treeModel ){
        m_treeModel->parse(m_root);
        m_treeModel->setSelected(m_current);
    }
}

void QCodeBase::save(){
    if ( m_root->hasInsertions() ){
        m_root->saveInsertions();

        QString selectionTypeString = m_current->typeName();
        QString selectionIdentifier = m_current->identifier();

        CXTranslationUnit transUnit = m_classifier->translatinoUnit();
        clang_reparseTranslationUnit(transUnit, 0, 0, clang_defaultReparseOptions(transUnit));
        CXCursor startCursor        = clang_getTranslationUnitCursor(transUnit);
        m_classifier->reparse();
        if ( m_treeModel )
            m_treeModel->clearAndReset();
        m_root->removeChildren();
        QASTVisitor::createCSANodeTree(startCursor, m_root, m_classifier);
        m_current = m_root;
        if ( m_treeModel ){
            m_treeModel->parse(m_root);
            m_treeModel->setSelected(m_current);
        }
        select(selectionTypeString, selectionIdentifier);
    }
}

void QCodeBase::propagateUserCursor(int offset, const QString &file){
    CXTranslationUnit transUnit = m_classifier->translatinoUnit();
    CXFile cfile = clang_getFile(transUnit, file.toStdString().c_str());
    CXSourceLocation sLocation = clang_getLocationForOffset(transUnit, cfile, offset);
    propagateUserCursor(QSourceLocation(sLocation));
}

void QCodeBase::propagateUserCursor(int line, int column, const QString &file){
    CXTranslationUnit transUnit = m_classifier->translatinoUnit();
    CXFile cfile = clang_getFile(transUnit, file.toStdString().c_str());
    CXSourceLocation sLocation = clang_getLocation(transUnit, cfile, line, column);
    propagateUserCursor(QSourceLocation(sLocation));
}

void QCodeBase::propagateUserCursor(const QSourceLocation &location){
    QASTNode* deepest = m_root->propagateUserCursor(location);
    if ( deepest != 0 ){
        m_current = deepest;
        if ( m_treeModel )
            m_treeModel->setSelected(m_current);
    }
}

bool QCodeBase::select(const QString &typeString, const QString &name){
    QASTNode* result = qobject_cast<QASTNode*>(m_root->find(typeString, name));
    if ( result ){
        m_current = result;
        if ( m_treeModel )
            m_treeModel->setSelected(m_current);
        return true;
    }
    return false;
}

QList<QASTNode*> QCodeBase::files(){
    return m_files;
}

QASTNode* QCodeBase::cursorNode(){
    return m_current;
}

}// namespace