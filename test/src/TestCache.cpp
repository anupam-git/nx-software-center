//
// Created by alexis on 24/01/18.
//

#include <QFile>

#include <gtest/gtest.h>

#include "entities/Cache.h"
#include "entities/Repository.h"

namespace NX_SOFTWARE_CENTER_TESTS {
    class TestCache : public testing::Test {
    };

    TEST_F(TestCache, addApplication) {
        Repository repository;

        Cache cache(&repository);
        QString cacheDir = cache.getApplicationsCachePath();

        Application a("test", "1.0");
        repository.add(a);

        cache.handleInstalledApplicationsChanged({"test-1.0"});

        QString cacheFilePath = cacheDir + a.getId() + ".json";
        QFile f(cacheFilePath);

        QByteArray expected = R"({"codeName":"test","id":"test-1.0","version":"1.0"})";

        if (f.open(QIODevice::ReadOnly)) {
            QByteArray data = f.readAll();
            ASSERT_EQ(expected, data);
            f.close();
        } else
            GTEST_FATAL_FAILURE_("Unable to open cache file: " + cacheFilePath.toLocal8Bit());

        QFile::remove(cacheFilePath);
    }

    TEST_F(TestCache, removeApplication) {
        Repository repository;

        Cache cache(&repository);
        QString cacheDir = cache.getApplicationsCachePath();

        Application a("test1", "1.0");
        Application b("test2", "1.0");

        repository.add(a);
        repository.add(b);

        cache.handleInstalledApplicationsChanged({"test1-1.0"});

        QString appAAacheFilePath = cacheDir + a.getId() + ".json";
        QString appBCacheFilePath = cacheDir + b.getId() + ".json";
        ASSERT_TRUE(QFile::exists(appAAacheFilePath));
        ASSERT_FALSE(QFile::exists(appBCacheFilePath));

        QFile::remove(appAAacheFilePath);
        QFile::remove(appBCacheFilePath);
    }
}