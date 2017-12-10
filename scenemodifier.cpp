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

#include "scenemodifier.h"

#include <QtCore/QDebug>
#include <Qt3DRender>
#include <Qt3DRender/QMesh>

#include<Qt3DExtras/QPerVertexColorMaterial>
#include <Qt3DRender/QAttribute>

#include <cmath>
#include <ctime>



SceneModifier::SceneModifier(Qt3DCore::QEntity *rootEntity)
    : m_rootEntity(rootEntity)
{  
    //create and draw parent node
    spheres.SetParent( new Node( QVector3D(0,8,0), RADROOT,spheres.colors[0]) );
    auto parent = spheres.parent;
    DrawSphere(parent->center,parent->colour,parent->radius);


    spheres.GenerateRandNodes(1,parent);

    spheres.GenerateNodes(2,parent->children);

    spheres.Draw(this,parent->children,parent);

}

SceneModifier::~SceneModifier()
{

}

void SceneModifier::DrawLine(const QVector3D& a,const QVector3D& b)
{
    Qt3DRender::QGeometryRenderer * meshRenderer = new Qt3DRender::QGeometryRenderer;
    Qt3DRender::QGeometry * geometry = new Qt3DRender::QGeometry(meshRenderer);

    Qt3DRender::QBuffer * vertexDataBuffer = new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, geometry);

    int vertexNum = 6*2;

    float* vertexRawData = new float[vertexNum];
    int idx = 0;
    QColor majorColor = QColor(220,220,220);

    vertexRawData[idx++] = a.x();
    vertexRawData[idx++] = a.y();
    vertexRawData[idx++] = a.z();

    vertexRawData[idx++] = majorColor.redF();
    vertexRawData[idx++] = majorColor.greenF();
    vertexRawData[idx++] = majorColor.blueF();

    vertexRawData[idx++] = b.x();
    vertexRawData[idx++] = b.y();
    vertexRawData[idx++] = b.z();
    vertexRawData[idx++] = majorColor.redF();
    vertexRawData[idx++] = majorColor.greenF();
    vertexRawData[idx++] = majorColor.blueF();


    QByteArray ba;
    int bufferSize = vertexNum * sizeof(float);
    ba.resize(bufferSize);
    memcpy(ba.data(), reinterpret_cast<const char*>(vertexRawData), bufferSize);
    vertexDataBuffer->setData(ba);

    int stride = 6 * sizeof(float);

    // Attributes
    Qt3DRender::QAttribute *positionAttribute = new Qt3DRender::QAttribute();
    positionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    positionAttribute->setBuffer(vertexDataBuffer);
    positionAttribute->setDataType(Qt3DRender::QAttribute::Float);
    positionAttribute->setDataSize(3);
    positionAttribute->setByteOffset(0);
    positionAttribute->setByteStride(stride);
    positionAttribute->setCount(vertexNum / 2);
    positionAttribute->setName(Qt3DRender::QAttribute::defaultPositionAttributeName());


    Qt3DRender::QAttribute *colorAttribute = new Qt3DRender::QAttribute();
    colorAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
    colorAttribute->setBuffer(vertexDataBuffer);
    colorAttribute->setDataType(Qt3DRender::QAttribute::Float);
    colorAttribute->setDataSize(3);
    colorAttribute->setByteOffset(3 * sizeof(float));
    colorAttribute->setByteStride(stride);
    colorAttribute->setCount(vertexNum / 2);
    colorAttribute->setName(Qt3DRender::QAttribute::defaultColorAttributeName());

    geometry->addAttribute(positionAttribute);
    geometry->addAttribute(colorAttribute);

    meshRenderer->setInstanceCount(1);
    meshRenderer->setIndexOffset(0);
    meshRenderer->setFirstInstance(0);
    meshRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Lines);
    meshRenderer->setGeometry(geometry);
    meshRenderer->setVertexCount(vertexNum / 2);

    Qt3DExtras::QPerVertexColorMaterial * material = new Qt3DExtras::QPerVertexColorMaterial(m_rootEntity);
    Qt3DCore::QTransform *transform = new Qt3DCore::QTransform;
    transform->setScale(1.0f);

    Qt3DCore::QEntity *entity = new Qt3DCore::QEntity(m_rootEntity);
    entity->addComponent(meshRenderer);
    entity->addComponent(transform);
    entity->addComponent(material);

    entity->setParent(m_rootEntity);

    delete [] vertexRawData;

}

void SceneModifier::DrawSphere(const QVector3D& a,QColor color,float radius)
{
    // Sphere shape data
    Qt3DExtras::QSphereMesh *sphereMesh = new Qt3DExtras::QSphereMesh();
    sphereMesh->setRings(20);
    sphereMesh->setSlices(20);
    sphereMesh->setRadius(radius);

    // Sphere mesh transform
    Qt3DCore::QTransform *sphereTransform = new Qt3DCore::QTransform();

    sphereTransform->setScale(1.0f);
    sphereTransform->setTranslation(a);

    Qt3DExtras::QPhongMaterial *sphereMaterial = new Qt3DExtras::QPhongMaterial();
    sphereMaterial->setDiffuse(color);

    // Sphere
    Qt3DCore::QEntity *sphereEntity = new Qt3DCore::QEntity(m_rootEntity);
    sphereEntity->addComponent(sphereMesh);
    sphereEntity->addComponent(sphereMaterial);
    sphereEntity->addComponent(sphereTransform);
}

void SceneModifier::Tree::CreateColors()
{
    colors.push_back(QColor(0,255,0));
    colors.push_back(QColor(0,0,255));
    colors.push_back(QColor(255,0,0));
    colors.push_back(QColor(255,127,25));
    colors.push_back(QColor(64,64,25));
    colors.push_back(QColor(127,127,127));
    colors.push_back(QColor(14,60,90));
}

SceneModifier::Tree::Tree()
{
    CreateColors();
}

SceneModifier::Tree::~Tree()
{
    delete parent;
}

void SceneModifier::Tree::SetParent(SceneModifier::Node *root)
{
    parent = root;
}

void SceneModifier::Tree::Draw(SceneModifier * const sc, QVector<Node*>children, Node* parent)
{
    if(children.size() == 0)
        return;

    for(size_t i = 0; i < children.size(); ++i)
    {
        sc->DrawSphere(children[i]->center,children[i]->colour,children[i]->radius);
        sc->DrawLine(parent->center,children[i]->center);
    }

    for(size_t i = 0; i< children.size(); ++i)
    {
        Draw(sc,children[i]->children,children[i]);
    }

}

void SceneModifier::Tree::GenerateRandNodes(int layer,Node * par)
{
    std::mt19937 gen;
    gen.seed(std::random_device{}());
    std::uniform_int_distribution<> dis(1, NMAX);
    int nodes = dis(gen);
    if( nodes > PLANESIZE)
    {
        GeneratePlaneSpheres(par,nodes,layer);
    }
    else
    {
        QVector3D translpoint(par->center.x(),par->center.y() - 30*par->radius,par->center.z() );

        std::uniform_real_distribution<double> xDistr(translpoint.x() - 20*par->radius, translpoint.x() + 20*par->radius);
        std::uniform_real_distribution<double> yDistr(translpoint.y() - 20*par->radius, translpoint.y() + 20*par->radius);
        std::uniform_real_distribution<double> zDistr(translpoint.z() - 20*par->radius, translpoint.z() + 20*par->radius);

        QVector <Node*> candidates;
        while( candidates.size() < nodes)
        {
            QVector3D temp(xDistr(gen),yDistr(gen),zDistr(gen));
            auto candidate = new Node(temp,RADNODE,colors[layer]);
            if( ! CollideOrExist(candidate,parent) )
            {
                candidates.push_back(candidate);
            }

        }

        for(size_t i = 0; i< candidates.size(); ++i)
        {
             par->AddChild(candidates[i]);
        }
    }
}

void SceneModifier::Tree::GeneratePlaneSpheres(Node* const  par, const int &nodes, const int &layer)
{
    QVector<Node *> candidates = CreateCandidates(par,layer);
    CreateRestOnes(par,nodes,candidates,layer);

    for(size_t i = 0; i< candidates.size(); ++i)
    {
         par->AddChild(candidates[i]);
    }
}

double SceneModifier::Tree::CalcA(const QVector3D &A, const QVector3D &B, const QVector3D &C)
{
    return (B.y() - A.y()) * (C.z() - A.z()) - (C.y() - A.y()) * (B.z()-A.z());
}

double SceneModifier::Tree::CalcB(const QVector3D &A,const QVector3D &B,const QVector3D &C)
{
    return (B.z() - A.z()) * (C.x() - A.x()) - (C.z() - A.z()) * (B.x()-A.x());
}

double SceneModifier::Tree::CalcC(const QVector3D &A,const QVector3D &B,const QVector3D &C)
{
    return (B.x() - A.x()) * (C.y() - A.y()) - (C.x() - A.x()) * (B.y()-A.y());
}

double SceneModifier::Tree::CalcD(const QVector3D &A,const double& a,const double& b, const double& c)
{
    return -(a*A.x() + b*A.y() + c*A.z());
}

bool SceneModifier::Tree::CollideOrExist(const SceneModifier::Node *const node, Node * const inner )
{
    if(inner->CheckCollide(node))
        return true;

    for(size_t i = 0; i < inner->children.size(); ++i){
        if ( CollideOrExist(node,inner->children[i]) ){
            return true;
        }
    }
    return false;
}

QVector<SceneModifier::Node *> SceneModifier::Tree::CreateCandidates(const Node * const par, const int& layer)
{
    std::mt19937 gen;
    gen.seed(std::random_device{}());

    bool isplane = false;
    QVector<Node *> candidates;
    while(!isplane)
    {

        QVector3D translpoint(par->center.x(),par->center.y() - 30*par->radius,par->center.z() );

        std::uniform_real_distribution<double> xDistr(translpoint.x() - 20*par->radius, translpoint.x() + 20*par->radius);
        std::uniform_real_distribution<double> yDistr(translpoint.y() - 20*par->radius, translpoint.y() + 20*par->radius);
        std::uniform_real_distribution<double> zDistr(translpoint.z() - 20*par->radius, translpoint.z() + 20*par->radius);

        while(candidates.size() < PLANESIZE)
        {
            QVector3D temp(xDistr(gen),yDistr(gen),zDistr(gen));
            auto cand = new Node(temp,RADNODE,colors[layer]);

            if(!CollideOrExist(cand,parent))
            {
                candidates.push_back(cand);
            }
            else
            {
                delete cand;
            }
        }

        auto A = candidates[0]->center;
        auto B = candidates[1]->center;
        auto C = candidates[2]->center;

        // ax+by+cz+d = 0 =>  x = (-d - cz - by)/a;  y = (-d - cz - ax)/b; ; z = (-d - ax - by)/c ;
        double a = CalcA(A,B,C);
        double b = CalcB(A,B,C);
        double c = CalcC(A,B,C);


        isplane = a != 0 || b != 0 || c!= 0;
    }
    return candidates;
}

void SceneModifier::Tree::CreateRestOnes(Node * par,const int& nodes, QVector<Node *> & candidates,const int& layer)
{
    std::mt19937 gen;
    gen.seed(std::random_device{}());

    auto A = candidates[0]->center;
    auto B = candidates[1]->center;
    auto C = candidates[2]->center;

    // ax+by+cz+d = 0 =>  x = (-d - cz - by)/a;  y = (-d - cz - ax)/b; ; z = (-d - ax - by)/c ;
    double a = CalcA(A,B,C);
    double b = CalcB(A,B,C);
    double c = CalcC(A,B,C);
    double d = CalcD(A,a,b,c);

    while(candidates.size() < nodes)
    {
        Node * cand = 0;

        QVector3D translpoint(par->center.x(),par->center.y() - 25*par->radius,par->center.z() );

        std::uniform_real_distribution<double> xDistr(translpoint.x() - 20*par->radius, translpoint.x() + 20*par->radius);
        std::uniform_real_distribution<double> yDistr(translpoint.y() - 20*par->radius, translpoint.y() + 20*par->radius);
        std::uniform_real_distribution<double> zDistr(translpoint.z() - 20*par->radius, translpoint.z() + 20*par->radius);
        if(a != 0)
        {
            double rand_y = yDistr(gen);
            double rand_z = zDistr(gen);
            double x = ( -d - b*rand_y - c*rand_z) / a;

            QVector3D temp(x,rand_y, rand_z);
            cand = new Node(temp,RADNODE,colors[layer]);
        }
        else if(c !=  0)
        {
            double rand_x = xDistr(gen);
            double rand_y = yDistr(gen);
            double z = ( -d - a*rand_x - b*rand_y) / c;

            QVector3D temp(rand_x,rand_y, z);
            cand = new Node(temp,RADNODE,colors[layer]);
        }
        else if(b !=  0)
        {
            double rand_x = xDistr(gen);
            double rand_z = zDistr(gen);
            double y = ( -d - a*rand_x - c*rand_z) / b;

            QVector3D temp(rand_x,y, rand_z);
            cand = new Node(temp,RADNODE,colors[layer]);
        }

        if(!CollideOrExist(cand,parent))
        {
            candidates.push_back(cand);
        }
        else
        {
            if(cand)
                delete cand;
        }
    }
}

void SceneModifier::Tree::GenerateNodes(const int layer, QVector<Node*>children)
{
    if(layer >= L)
        return;

    for(size_t i = 0; i < children.size(); ++i)
    {
        GenerateRandNodes(layer,children[i]);
    }


    for(size_t i = 0; i < children.size(); ++i)
    {
        GenerateNodes(layer+1,children[i]->children);
    }

}
