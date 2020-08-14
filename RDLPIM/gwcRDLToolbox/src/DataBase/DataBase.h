#pragma once
#include"DBElement.h"
#include"RequestHandler/dataArray.hpp"
#include"RDL/RDL.hpp"

/// @brief The DataBase class forms the heart of the RDLPIM architecture, this singleton holds all data pushed into the RDLPIM as well as any subscribed data from the RDL itsself.
/// 
/// Ultimately, an instantiated DataBase object contains a dynamic array of std::string (data Name) and DBElement (actual data) key pairs. The String protion of the key pairs is a lookup name, for example th_temp_eff.
/// The DBElement class ultimately contains the binary for the data object stored, this can be any data type, primitive or custom defined.  The DBElement also keeps track of which variables are calculated within
/// the RDL, allowing any requests to this data to invoke a refresh from the RDL as a source.
class DataBase
{
public:
	~DataBase() = default;

	/// @brief This method instantiates an instance of the DataBase class if it hasnt already so, and returns a raw pointer to it.
	/// @return A raw pointer to a DataBase instance.
	static DataBase* GetInstance();

	/// @brief A method to modify or add a new data element to the DataBAse.
	/// 
	/// The variables name is extracted from the DataElement object and a lookup to the RDL is performed. If the data exists within the RDL's runtime memory, 
	/// it is updated with the value passed by this function, if not, the relevant DBElement is found and the data added/updated.  Once a DBElement has been succesfully modified, an event is raised from that element broadcasting that it has changed.
	/// @param data a reference to a DataElement object.
	/// 
	/// @warning if you are trying to add multiple dataElements to the DataBase, do not repeatedly use this method as it will raise excessive onDataChanged events and therefore many small subscriptions packets, this is not performant.
	void ModData(const DataElement& data);

	/// @brief A method to modify many DBElements at once. 
	/// 
	/// The DataElementArray is deserialised into an array of DataElements, these are then used to modify many DBElements simultaneously.  After completion a single event is raised with data for all modified members (DataBase::onChanged), this will result in fewer but more
	/// densly packed subscription packets being sent to subscribed clients.
	/// @param dataArr DataElementArray refernce (containing multiple DataElements.)
	void ModData(const DataElementArray& dataArr);

	/// @brief A method to determine if a variable exists within the DataBase instance
	/// @param name Variable name
	/// @return True if exists, False if not
	bool PointExists(const std::string& name);
	//void RemoveData(const DataElement& data);
	
	/// @brief A method to get access to the underlying data within a DBElement of the DataBase Class.
	/// @param varname Variable name
	/// @return A copy of the DataElement.
	DataElement GetData(const std::string& varname);

	/// @brief A method to return a shared pointer reference to a DBElement within the DataBase instance.
	/// @param name Variable name
	/// @return a shared pointer reference to a DBElement
	Ref<DBElement>& GetEntry(const std::string& name) { return m_Data[name]; }


	/// @brief A method to get a constant reference to the onNewEntry event
	/// @return const reference to the onNewEntry event
	Event<const std::string&>& GetOnNewEntry() { return OnNewEntry; }



private://members
	//std::unordered_map<Buffer, Ref<DBElement>> m_Data;
	std::unordered_map<std::string, Ref<DBElement>> m_Data;

	static DataBase* s_Instance;
private://methods
	DataBase();
	
	std::mutex m_DBAccess;

	Event<const std::string&> OnNewEntry;
	Event<const DataElementArray&> OnUpdated;
};