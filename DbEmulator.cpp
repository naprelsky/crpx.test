#include <iostream>
#include <fstream>
#include <vector>

#include <sstream>
#include <algorithm>
#include <iterator>

#include "DbEmulator.h"


DbEmulator::DbEmulator()
{
    loadData();
}

void DbEmulator::loadData()
{
    StringsVector lines;

    // Loading legal persons data
    lines = service::loadFileToVector("TextData\\LegalPersons.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        Person element = { stoi(lineData.at(0)), PERSON_TYPE::LEGAL, encoding::utf8_to_cp1251(lineData.at(1)) };
        legalPersons.addRow(element.ID, element);
    }

    // Loading individuals data
    lines = service::loadFileToVector("TextData\\Individuals.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        Person element = { stoi(lineData.at(0)) , PERSON_TYPE::INDIVIDUAL, encoding::utf8_to_cp1251(lineData.at(1)) };
        individuals.addRow(element.ID, element);
    }

    // Loading ownership stake data
    lines = service::loadFileToVector("TextData\\OwnershipStake.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        OwnershipStake element = { stoi(lineData.at(0)),    // ID
                                   stoi(lineData.at(1)),    // CompanyID
                                   stoi(lineData.at(2)),    // CompanyType
                                   stoi(lineData.at(3)),    // OwnerID
                                   stoi(lineData.at(4)),    // OwnerType
                                   stof(lineData.at(5))     // Share
                                 };
        ownershipStake.addRow(element.ID, element);
    }

    // Loading single executive data
    lines = service::loadFileToVector("TextData\\SingleExecutive.txt");
    for (int i = 0; i < lines.size(); ++i) {
        StringsVector lineData = service::split(lines.at(i), '_');

        SingleExecutive element = { stoi(lineData.at(0)),   // ID
                                    stoi(lineData.at(1)),    // CompanyID
                                    stoi(lineData.at(2)),    // CompanyType
                                    stoi(lineData.at(3)),    // OwnerID
                                    stoi(lineData.at(4)),    // OwnerType
                                  };
        singleExecutive.addRow(element.ID, element);
    }
}


StringsVector DbEmulator::getAllPersonsName()
{
    StringsVector result;

    for (auto it = legalPersons.begin(); it != legalPersons.end(); ++it) {
        result.emplace_back(it->second.Name);
    }

    for (auto it = individuals.begin(); it != individuals.end(); ++it) {
        result.emplace_back(it->second.Name);
    }

    return result;
}

std::vector<ReportItem> DbEmulator::getReport(const std::string& companyName)
{
    std::vector<ReportItem> result;

    // Поиск юридического лица по наименованию
    Person selected;
    for (auto it = legalPersons.begin(); it != legalPersons.end(); ++it) {
        if (it->second.Name == companyName) {
            selected = it->second;
            break;
        }
    }

    if (selected.Name.empty()) {
        for (auto it = individuals.begin(); it != individuals.end(); ++it) {
            if (it->second.Name == companyName) {
                selected = it->second;
                break;
            }
        }
    }

    // Поиск юридических/физических лиц по степени участия в компании (более 50%)
    for (auto it = ownershipStake.begin(); it != ownershipStake.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (row.CompanyID == selected.ID && row.CompanyType == selected.Type && row.Share > 50) {
            if (row.OwnerType == PERSON_TYPE::LEGAL) {
                element.сompanyName = legalPersons.getRow(row.OwnerID).Name;
            }
            else if (row.OwnerType == PERSON_TYPE::INDIVIDUAL) {
                element.сompanyName = individuals.getRow(row.OwnerID).Name;
            }
        }
        else {
            continue;
        }

        element.relations = service::replace("%1  имеет право распоряжаться более 50% голосов %2", element.сompanyName.c_str(), selected.Name.c_str());
        element.basis = "пп.1 п.1 ст.9 з-на №135-ФЗ";
        result.emplace_back(element);
    }

    // Поиск юридических лиц, в которых выбранная компания имеет степерь участия более 50%
    for (auto it = ownershipStake.begin(); it != ownershipStake.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (selected.Type == PERSON_TYPE::LEGAL && row.OwnerID == selected.ID && row.Share > 50) {
            element.сompanyName = legalPersons.getRow(row.CompanyID).Name;
        }
        else {
            continue;
        }

        element.relations = service::replace("%1  имеет право распоряжаться более 50% голосов %2", selected.Name.c_str(), element.сompanyName.c_str());
        element.basis = "пп.1 п.1 ст.9 з-на №135-ФЗ";
        result.emplace_back(element);
    }

    // Поиск юридических/физических лиц, являющихся ЕИО компании 
    for (auto it = singleExecutive.begin(); it != singleExecutive.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (row.CompanyID == selected.ID && row.CompanyType == selected.Type) {
            if (row.OwnerType == PERSON_TYPE::LEGAL) {
                element.сompanyName = legalPersons.getRow(row.OwnerID).Name;
            }
            else if (row.OwnerType == PERSON_TYPE::INDIVIDUAL) {
                element.сompanyName = individuals.getRow(row.OwnerID).Name;
            }
        }
        else {
            continue;
        }

        element.relations = service::replace("%1 является единоличным исполнительным органом %2", element.сompanyName.c_str(), selected.Name.c_str());
        element.basis = "пп.2 п.1 ст.9 з-на №135-ФЗ";
        result.emplace_back(element);
    }

    // Поиск юридических лиц, в которых выбранная компания является ЕИО 
    for (auto it = singleExecutive.begin(); it != singleExecutive.end(); ++it)
    {
        ReportItem element;

        auto row = it->second;
        if (row.CompanyType == PERSON_TYPE::LEGAL && row.OwnerID == selected.ID && row.OwnerType == selected.Type) {
            element.сompanyName = legalPersons.getRow(row.CompanyID).Name;
        }
        else {
            continue;
        }

        element.relations = service::replace("%1 является единоличным исполнительным органом %2", selected.Name.c_str(), element.сompanyName.c_str());
        element.basis = "пп.2 п.1 ст.9 з-на №135-ФЗ";
        result.emplace_back(element);
    }

    return result;
}

