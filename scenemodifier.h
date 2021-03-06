/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef SCENEMODIFIER_H
#define SCENEMODIFIER_H

#include <QtCore/QObject>

#include <Qt3DCore/qentity.h>
#include <Qt3DCore/qtransform.h>

#include <Qt3DExtras/QTorusMesh>
#include <Qt3DExtras/QConeMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QPlaneMesh>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DExtras/QPhongMaterial>
#include<Qt3DRender/QMesh>


class SceneModifier : public QObject
{
    Q_OBJECT

public:
    explicit SceneModifier(Qt3DCore::QEntity *rootEntity);
    ~SceneModifier();

public slots:


private:
    Qt3DCore::QEntity *m_rootEntity;
    struct Node{

        QVector< Node *> children;
        QVector3D center;
        float radius;
        QColor colour;

        Node* AddChild(Node *child)
        {
            children.push_back(child);
            return child;
        }

        bool CheckCollide(const Node* const sphere2)
        {
            auto distance = this->center.distanceToPoint(sphere2->center);
            return distance <= this->radius + sphere2->radius;
        }

        Node(QVector3D cent,float rad,QColor color)
            :center(cent),radius(rad),colour(color)
        {

        }

        ~Node()
        {
            for (auto it = children.begin() ; it != children.end(); ++it)
            {
              delete (*it);
            }
            children.clear();
        }

    };

    class Tree {
    public:
        Node * parent;
        QVector<QColor> colors;
        const int PLANESIZE = 3;
    public:
        Tree();
        ~Tree();
        Tree(Node root,QVector< Node *> child);
        void SetParent(Node *root);
        void Draw(SceneModifier *const, QVector<Node *> children, Node *parent);
        void GenerateRandNodes(int layer,Node * par);
        void GenerateNodes(const int layer,QVector<Node*>children);
        void CreateColors();
        QVector<Node *> CreateCandidates(const Node * const par, const int& layer);
        void GeneratePlaneSpheres(Node * const par, const int& nodes, const int& layer);
        void CreateRestOnes(Node *,const int&, QVector<Node *> &, const int&);
        //plane equation coefficients
        double CalcA(const QVector3D& A, const QVector3D& B, const QVector3D& C);
        double CalcB(const QVector3D& A, const QVector3D& B, const QVector3D& C);
        double CalcC(const QVector3D& A, const QVector3D& B, const QVector3D& C);
        double CalcD(const QVector3D& A, const double &a, const double &b, const double &c);
        bool CollideOrExist(const Node* const node, Node * const inner);
    } spheres;

    static constexpr int NMAX = 5;
    static constexpr int L = 3;
    static constexpr float RADNODE = 0.1;
    static constexpr float RADROOT = 0.3;
private:
    void DrawLine(const QVector3D&,const QVector3D& );
    void DrawSphere(const QVector3D& ,QColor,float );

};





#endif // SCENEMODIFIER_H

