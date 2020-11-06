﻿#include "src/coreset.h"
#include "src/filesqlinfo.h"

#include "src/episodes.h"
#include "src/shot.h"
#include "src/shotClass.h"
#include "src/shottype.h"
#include "src/assClass.h"

#include "src/assfilesqlinfo.h"
#include "src/shotfilesqlinfo.h"

#include <gtest/gtest.h>


#include <iostream>
class CoreTest : public ::testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    doCore::coreSet &set = doCore::coreSet::getCoreSet();
};

void CoreTest::SetUp()
{
    set.init();
    set.setProjectname("dubuxiaoyao3");
    set.initdb();
}

void CoreTest::TearDown()
{
    
}

TEST_F(CoreTest, tets_quert)
{
    std::cout << set.getCacheRoot().absolutePath().toStdString() << std::endl;
    RecordProperty("cacheRoot", set.getCacheRoot().absolutePath().toStdString());
}

TEST_F(CoreTest, set_synpath)
{
    for (doCore::synPath_struct &p : set.getSynDir())
    {
        std::cout << "\n local -->" << p.local.toStdString()
                  << "\n server-->" << p.server.toStdString() << std::endl;
        ;
    }
}

TEST_F(CoreTest, create_shotinfo)
{
    doCore::episodesPtrList eplist;
    eplist = doCore::episodes::getAll();
    if (eplist.size() < 2)
    {
        doCore::episodesPtr eps(new doCore::episodes());
        eps->setEpisdes(11);
        eps->insert();

        doCore::shotPtr sh(new doCore::shot());
        sh->setShot(10);
        sh->setEpisodes(eps.toWeakRef());
        sh->insert();

        doCore::shotClassPtr fc(new doCore::shotClass());
      fc->setclass(doCore::fileClass::e_fileclass::VFX);

        fc->setShot(sh.toWeakRef());
        fc->insert();

        doCore::shotTypePtr ft(new doCore::shotType());
      ft->setType("test");
        ft->setFileClass(fc.toWeakRef());

        ft->insert();

        doCore::shotInfoPtr sf(new doCore::shotFileSqlInfo());
        doCore::QfileInfoVector list;
        sf->setInfoP(QString("test"));
        list.append(QFileInfo("D:/tmp/etr.vdb"));
        sf->setFileList(list);
        sf->setVersionP(0);

      sf->setShotType(ft.toWeakRef());
        sf->insert();
    }
    else
    {
        std::cout << "is create ok, " << std::endl;
    }
}

TEST_F(CoreTest, get_shotinf)
{
    doCore::episodesPtrList eplist;
    eplist = doCore::episodes::getAll();
    if (!eplist.isEmpty())
    {
        doCore::episodesPtr ep = eplist[0];
        doCore::shotPtr sh = doCore::shot::getAll(ep)[0];
        doCore::shotClassPtr fc = doCore::fileClass::getAll(sh)[0];
        doCore::shotTypePtr ft = doCore::fileType::getAll(fc)[0];
        doCore::shotInfoPtr sf = doCore::shotFileSqlInfo::getAll(ft)[0];

        std::cout << "episodes: " << ep->getEpisdes_str().toStdString() << std::endl;
        std::cout << "shot:" << sh->getShotAndAb_str().toStdString() << std::endl;
        std::cout << "fileclass :" << fc->getClass_str().toStdString() << std::endl;
        std::cout << "filetype :" << ft->getType().toStdString() << std::endl;
        std::cout << "shotinfo generatePath :" << sf->generatePath("test", ".mb").toStdString() << std::endl;
        for (auto &x : sf->getFileList())
        {
            std::cout << "shotinfo path :" << x.absoluteFilePath().toStdString() << std::endl;
        }
    }
}

TEST_F(CoreTest, create_assInfo)
{
    doCore::shotClassPtrList fc_ = doCore::fileClass::getAll();

    if (fc_.size() == 4)
    {
        doCore::assClassPtr af_(new doCore::assClass);
        af_->setFileClass(fc_[0]);
      af_->setAssClass(QString::fromUtf8("大小"), true);
        af_->insert();

        doCore::shotTypePtr ft_(new doCore::shotType);
      ft_->setType("ffff");
        ft_->setAssType(af_);
        ft_->insert();

        doCore::assInfoPtr sf_(new doCore::assFileSqlInfo);
        doCore::QfileInfoVector list;
        sf_->setInfoP(QString("test"));
        list.append(QFileInfo("D:/tmp/etr.vdb"));
        sf_->setFileList(list);
        sf_->setVersionP(1);

      sf_->setAssClass(ft_);

        sf_->insert();
    }
}

TEST_F(CoreTest, get_assInf)
{
    doCore::shotClassPtrList list_fileClass;
    list_fileClass = doCore::fileClass::getAll();
    for (auto &&x : list_fileClass)
    {
        std::cout << "fileclass :" << x->getClass_str().toStdString() << std::endl;
    }

    doCore::assClassPtr af_ = doCore::assType::getAll(list_fileClass[0])[0];
    std::cout << "asstype :" << af_->getAssClass(true).toLocal8Bit().toStdString() << std::endl;
    RecordProperty("asstype", af_->getAssClass(true).toStdString());
    // QTextCodec *code = QTextCodec::codecForName("GBK");
    // std::cout << "asstype :" <<code->fromUnicode(af_->getAssClass(af_)).toStdString() << std::endl;

    doCore::shotTypePtr ft_ = doCore::fileType::getAll(af_)[0];
    std::cout << "filetype :" << ft_->getType().toStdString() << std::endl;

    doCore::assInfoPtr ai_ = doCore::assFileSqlInfo::getAll(ft_)[0];
    std::cout << "assinfo path :" << ai_->generatePath("test", ".mb").toStdString() << std::endl;
}
