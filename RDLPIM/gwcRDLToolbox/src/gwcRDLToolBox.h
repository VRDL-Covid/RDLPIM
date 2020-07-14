#pragma once
#include "rdlpch.h"
//connection manager object
#include"ConnectionManager/connectionManager.hpp"

//client manager object
#include"ClientManager/clientManager.hpp"

//request header generator
#include "RequestHandler/RequestHeader.h"

//RDL comms object
#include"RDL/RDL.hpp"

//RDL data object to handle return from DBM lookup
#include"RDL/rdlData.hpp"

//database object to push, pull and subscribe to data transfer.
#include"RequestHandler/dataBase.hpp"

//basic events system ripped from gwcEngine.
#include"Core/Event.h"