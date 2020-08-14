#pragma once
#include"RequestHandler/DataElement.hpp"
#include "Core/Event.h"

/// @brief This class defines the structure of the data stored within a DataBase instance.
/// 
/// The DBElement object contains a DataElement object(the raw data), boolean to describe whether the data is live wihtin the RDL's runtime memory and an event which is raised upon the data within being modified.
class DBElement
{
public:
	DBElement(const DataElement& de):m_Data(de), m_RDLOwned(false){};
	~DBElement() = default;;

	/// @brief A method to get a constant reference to the underlying data contained within the DBElement object.
	/// @return A constant reference to a DataElement object.
	const DataElement& GetData()const { return m_Data; }

	/// @brief A method to set the contents of the DBElement object to that contained within a DataElement object
	/// 
	/// This method invokes the onChanged event, allowing those subscribed to know its value has changed.
	/// @param data the new data to be stored within the DBElement object
	void SetData(const DataElement& data) { m_Data = data; m_OnChanged.raiseEvent(m_Data); }

	/// @brief A method to set the contents of the DBElement object to data stripped from the RDL's runtime memory.
	/// 
	/// This method handls type setting and sizing of the DBElement dynamically with the support of GSE's DBM
	/// @param data an rdlData object reference, this object is responsible for interogating the RDL's process memory safely.
	void SetData(const rdlData& data) { m_Data.set(data); m_RDLOwned = true; m_OnChanged.raiseEvent(m_Data); }

	/// @brief A method to return whether or not the data contained within the DBElement object is live within the RDL's runtime memory.
	/// @return True if is RDL owned/Calculated, False if not.
	const bool IsRDLOwned()const { return m_RDLOwned; }

	/// @brief A method to specify that the data contained within the DBElement object is lived within the RDL's runtime memory
	/// @param state True if is RDL owned/Calculated, False if not.
	void SetRDLOwned(bool state) { m_RDLOwned = state; }

	/// @brief A method to get a const reference to the onChanged event.
	/// 
	/// This is required so that a developer may subscribe a call back to it.
	/// @return 
	Event<const DataElement&>& GetOnChangedEvent() { return m_OnChanged; }

private:
	DataElement m_Data;
	bool m_RDLOwned;
	Event<const DataElement&> m_OnChanged;
};
