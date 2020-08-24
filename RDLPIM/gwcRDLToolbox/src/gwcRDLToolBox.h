#pragma once
#include "rdlpch.h"
//connection manager object
#include"ConnectionManager/connectionManager.hpp"

//client manager object
#include"ClientManager/clientManager.hpp"

//request header generator
#include "RequestHandler/RequestHeader.h"
#include "RequestHandler/dataArray.hpp"

//RDL comms object
#include"RDL/RDL.hpp"

//RDL data object to handle return from DBM lookup
#include"RDL/rdlData.hpp"

//basic events system ripped from gwcEngine.
#include"Core/Event.h"

//RDLPIM Client helper class
#include"ConnectionManager/RDLPIM_Client.hpp"