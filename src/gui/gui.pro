QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    ../../thirdparty/pugixml/pugixml.cpp \
    ../common/bit_mask.cpp \
    ../common/logger.cpp \
    ../common/multimodal_communication/assotiation_mining/apriori.cpp \
    ../common/multimodal_communication/clustering/clope.cpp \
    ../common/multimodal_communication/mc_entities.cpp \
    ../common/multimodal_communication/transaction_mining/mc_by_time_slots_mining.cpp \
    ../common/rec/eaf/eaf_parser.cpp \
    ../common/rec/etf/etf_parser.cpp \
    ../common/rec/rec_entry/files/rec_entry_binary_file.cpp \
    ../common/rec/rec_entry/files/rec_entry_json_file.cpp \
    ../common/rec/rec_entry/rec_entry.cpp \
    ../common/rec/rec_template/files/rec_template_binary_file.cpp \
    ../common/rec/rec_template/files/rec_template_json_file.cpp \
    ../common/rec/rec_template/rec_template.cpp \
    casemodel.cpp \
    casewidget.cpp \
    casewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    pixeldelegate.cpp

HEADERS += \
    ../../thirdparty/cxxopts/cxxopts.hpp \
    ../../thirdparty/fmt/args.h \
    ../../thirdparty/fmt/chrono.h \
    ../../thirdparty/fmt/color.h \
    ../../thirdparty/fmt/compile.h \
    ../../thirdparty/fmt/core.h \
    ../../thirdparty/fmt/format-inl.h \
    ../../thirdparty/fmt/format.h \
    ../../thirdparty/fmt/locale.h \
    ../../thirdparty/fmt/os.h \
    ../../thirdparty/fmt/ostream.h \
    ../../thirdparty/fmt/printf.h \
    ../../thirdparty/fmt/ranges.h \
    ../../thirdparty/nlohmann/nlohmann.hpp \
    ../../thirdparty/pugixml/pugiconfig.hpp \
    ../../thirdparty/pugixml/pugixml.hpp \
    ../common/bit_mask.hpp \
    ../common/logger.hpp \
    ../common/multimodal_communication/assotiation_mining/apriori.hpp \
    ../common/multimodal_communication/clustering/clope.hpp \
    ../common/multimodal_communication/mc_entities.hpp \
    ../common/multimodal_communication/transaction_mining/mc_by_time_slots_mining.hpp \
    ../common/multimodal_communication/transaction_mining/mc_transaction_mining_interface.hpp \
    ../common/rec/eaf/eaf_data.hpp \
    ../common/rec/eaf/eaf_parser.hpp \
    ../common/rec/etf/etf_data.hpp \
    ../common/rec/etf/etf_parser.hpp \
    ../common/rec/rec_entry/files/rec_entry_binary_file.hpp \
    ../common/rec/rec_entry/files/rec_entry_json_file.hpp \
    ../common/rec/rec_entry/rec_entry.hpp \
    ../common/rec/rec_template/files/rec_template_binary_file.hpp \
    ../common/rec/rec_template/files/rec_template_json_file.hpp \
    ../common/rec/rec_template/rec_template.hpp \
    casemodel.hpp \
    casewidget.hpp \
    casewindow.hpp \
    mainwindow.hpp \
    pixeldelegate.hpp

INCLUDEPATH += \
    "../../thirdparty" \
    "../common" \
    "../common/rec" \
    "../common/rec/eaf" \
    "../common/rec/etf" \
    "../common/rec/rec_entry" \
    "../common/rec/rec_entry/files" \
    "../common/rec/rec_template" \
    "../common/rec/rec_template/files" \
    "../common/multimodal_communication" \
    "../common/multimodal_communication/transaction_mining" \
    "../common/multimodal_communication/assotiation_mining" \
    "../common/multimodal_communication/clustering"

DEFINES += \
    FMT_HEADER_ONLY

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
