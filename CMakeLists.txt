cmake_minimum_required(VERSION 3.16)
project(veracryptrunner)

set(CMAKE_PROJECT_VERSION "0.0.1")
set(CMAKE_PROJECT_HOMEPAGE_URL "https://github.com/alex1701c/krunner-veracryptrunner")
set(CMAKE_PROJECT_AUTHOR "alex1701c")
set(CMAKE_PROJECT_AUTHOR_EMAIL "alex1701c.dev@gmx.net")

set(QT_MIN_VERSION 5.15.0)
set(KF_MIN_VERSION 5.105.0)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_MODULE_PATH})
include(KDEInstallDirs)
include(KDEClangFormat)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMGenerateDBusServiceFile)
include(ECMDeprecationSettings)
include(FeatureSummary)

remove_definitions(-DQT_NO_CAST_FROM_ASCII)
ecm_set_disabled_deprecation_versions(
    QT ${QT_MIN_VERSION}
    KF ${KF_MIN_VERSION}
)

find_package(Qt${QT_MAJOR_VERSION} ${QT_MIN_VERSION} REQUIRED CONFIG COMPONENTS DBus Widgets)
find_package(KF${QT_MAJOR_VERSION} ${KF_MIN_VERSION} REQUIRED COMPONENTS Config Notifications I18n KCMUtils)

add_subdirectory(src)

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Krunner Plugin which allows you to interact with the NordVPN CLI")

# CPack configuration
set(CPACK_PACKAGE_NAME krunner-${CMAKE_PROJECT_NAME})
set(CPACK_PACKAGE_VERSION "${CMAKE_PROJECT_VERSION}")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "${CMAKE_PROJECT_AUTHOR}")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")

set(KRUNNER_VERSION "${KF${QT_MAJOR_VERSION}_VERSION}")
set(CPACK_PACKAGE_FILE_NAME ${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_OS}-plasma${QT_MAJOR_VERSION})
if (CPACK_OS STREQUAL "ubuntu")
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libkf${QT_MAJOR_VERSION}runner${QT_MAJOR_VERSION} (>=${KRUNNER_VERSION})")
	set(CPACK_GENERATOR "DEB")
elseif(CPACK_OS STREQUAL "opensuse" AND QT_MAJOR_VERSION STREQUAL 5)
	set(CPACK_RPM_PACKAGE_REQUIRES "libKF5Runner5 >= ${KRUNNER_VERSION}")
	set(CPACK_GENERATOR "RPM")
elseif(CPACK_OS STREQUAL "fedora" OR CPACK_OS STREQUAL "opensuse")
	set(CPACK_RPM_PACKAGE_REQUIRES "kf${QT_MAJOR_VERSION}-krunner >= ${KRUNNER_VERSION}")
	set(CPACK_GENERATOR "RPM")
else()
  unset(CPACK_PACKAGE_FILE_NAME)
endif()
include(CPack)

feature_summary(WHAT ALL INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES)
