/*
#include "CTasksProcessor.h"
#include "CImportProcessor.h"
#include "stored_proc_query_builder.h"
#include "XmlTools.h"
#include "IKernel.h"
#include "PTException.h"

#include <algorithm>

#include <boost/bind.hpp>
#include <boost/property_tree/ptree.hpp>

//! Node name containing import data
static const wchar_t CATEGORY_ROOT_NODE_NAME[] = L"tasks";

//! Node name for each rule data
static const wchar_t ENTITY_NODE_NAME[]   = L"task";

//! Rule node name
static const wchar_t RULE_NODE_NAME[]   = L"task";

//! Profiles root
static const wchar_t PROFILES_ROOT_NODE_NAME[] = L"task_profiles";

//! Profile node
static const wchar_t PROFILE_NODE_NAME[]  = L"task_profile";

//! Hosts root
static const wchar_t HOSTS_ROOT_NODE_NAME[]  = L"hosts";

//! Host
static const wchar_t HOST_NODE_NAME[]   = L"host";

//! Overrides root
static const wchar_t OVERRIDES_ROOT_NODE_NAME[] = L"overrides";

//! Override parameter
static const wchar_t OVERRIDE_NODE_NAME[]  = L"parameter";

/*
//! Build sql script for control statuses param
void BuildControlStatuses(const xml::wide_t::value_type& xmlNode, PT::Function::Ptr& functionBuilder, unsigned long* const counter)
{
	if (xmlNode.first != STATUS_CONTROL_NODE_NAME)
		return;

	std::string name = "cs_soft_id";
	functionBuilder->param(name + PT::tostr(*counter), xmlNode.second.get<unsigned long>(ATTR(L"cs_soft_id")));
	name = "control_id";
	functionBuilder->param(name + PT::tostr(*counter), xmlNode.second.get<unsigned long>(ATTR(L"id")));
	name = "status";
	functionBuilder->param(name + PT::tostr(*counter), xmlNode.second.get<unsigned long>(ATTR(L"status")));

	++(*counter);
}

//! Build sql script for variables values
void BuildVariables(const xml::wide_t::value_type& xmlNode, PT::Function::Ptr& functionBuilder, unsigned long* const counter)
{
	if (xmlNode.first != VARIABLE_NODE_NAME)
		return;

	std::string name = "cs_soft_id";
	functionBuilder->param(name + PT::tostr(*counter), xmlNode.second.get<unsigned long>(ATTR(L"cs_soft_id")));
	name = "cs_variable_id";
	functionBuilder->param(name + PT::tostr(*counter), xmlNode.second.get<unsigned long>(ATTR(L"id")));
	name = "value";
	functionBuilder->param(name + PT::tostr(*counter), xmlNode.second.data().c_str());
	name = "ref_value";
	std::wstring refValue = xmlNode.second.get(ATTR(L"ref_value"), L"");
	if (!refValue.empty())
		functionBuilder->param(name + PT::tostr(*counter), refValue.c_str());

	++(*counter);
}

TasksProcessor::TasksProcessor(void) 
{
}

TasksProcessor::~TasksProcessor(void)
{
}

const wchar_t* TasksProcessor::GetCategoryNodeName() const
{
	return CATEGORY_ROOT_NODE_NAME;
}

const wchar_t* TasksProcessor::GetEntityNodeName() const
{
	return ENTITY_NODE_NAME;
}

const wchar_t* TasksProcessor::GetRuleNodeName() const
{
	return RULE_NODE_NAME;
}

bool TasksProcessor::GenerateReferenceFindCondition(const xml::wide_value_t& xmlEntity, std::wstring& nodeName, xml::wide_t& xmlResult) const
	{
	if (xmlEntity.first != ENTITY_NODE_NAME)
		return false;

	const std::wstring id = xmlEntity.second.get<std::wstring>(ATTR(L"id"));

	xmlResult.put(ATTR(L"id"), id);
	nodeName = ENTITY_NODE_NAME;

	return true;
}

void TasksProcessor::GenerateRequest() const
{
	PT::QueryBuilder sqlBuilder;
	sqlBuilder.func("tasks_load")->param("show_hidden", unsigned long(1));
	m_pResult->push_back(sqlBuilder.ToString());
}

void TasksProcessor::ProcessNode(const xml::wide_value_t& xmlNode)
{
	if (xmlNode.first != ENTITY_NODE_NAME)
	return;

	PT::QueryBuilder sqlBuilder;

	// getting action type
	ImportAction::Enum_t action = ImportAction::FromString(xmlNode.second.get(ATTR(L"action"), L""));

	// getting name
	const std::wstring name = xmlNode.second.get(ATTR(L"new_name"), xmlNode.second.get<std::wstring>(ATTR(L"name")));

	// getting destination id if exists
	const unsigned long id = xmlNode.second.get<unsigned long>(ATTR(L"destId"), xmlNode.second.get<unsigned long>(ATTR(L"id")));

	// checking and correcting action
	ValidateAction(action);

	if (ImportAction::Create == action)
	{
		/*
		// creating new
		sqlBuilder.func("cs_profile_create")
		->param("name", name.c_str())
		->earlyParam("\x02 cs_profile_id=");

		// building params
		boost::optional<const xml::wide_t&> statuses = xmlNode.second.get_child(STATUSES_ROOT_NODE_NAME);
		if (statuses)
		{
			PT::Function::Ptr funcPtr = sqlBuilder.func("cs_profile_modify_control_statuses");
			unsigned long counter = 0;
			std::for_each(statuses.get().begin(), statuses.get().end(), boost::bind(&BuildControlStatuses, _1, funcPtr, &counter));
			funcPtr->param("controls_count", counter);
		}

		boost::optional<const xml::wide_t&> variables = xmlNode.second.get_child(VARIABLES_ROOT_NODE_NAME);
		if (variables)
		{
			PT::Function::Ptr funcPtr = sqlBuilder.func("cs_profile_modify_variables");
			unsigned long counter = 0;
			std::for_each(variables.get().begin(), variables.get().end(), boost::bind(&BuildVariables, _1, funcPtr, &counter));
			funcPtr->param("variables_count", counter);
		}
	}
	else
	if (ImportAction::Modify == action)
	{
		/*
		// modifying
		sqlBuilder.func("cs_profile_clear_data")
		->param("cs_profile_id", id)
		->earlyParam("\x02 cs_profile_id=");

		// building params
		boost::optional<const xml::wide_t&> statuses = xmlNode.second.get_child(STATUSES_ROOT_NODE_NAME);
		if (statuses)
		{
			PT::Function::Ptr funcPtr = sqlBuilder.func("cs_profile_modify_control_statuses");
			unsigned long counter = 0;
			std::for_each(statuses.get().begin(), statuses.get().end(), boost::bind(&BuildControlStatuses, _1, funcPtr, &counter));
			funcPtr->param("controls_count", counter);
		}

		boost::optional<const xml::wide_t&> variables = xmlNode.second.get_child(VARIABLES_ROOT_NODE_NAME);
		if (variables)
		{
			PT::Function::Ptr funcPtr = sqlBuilder.func("cs_profile_modify_variables");
			unsigned long counter = 0;
			std::for_each(variables.get().begin(), variables.get().end(), boost::bind(&BuildVariables, _1, funcPtr, &counter));
			funcPtr->param("variables_count", counter);
		}
	}

	const std::string sql = sqlBuilder.ToString();
	if (!sql.empty())
		m_pResult->push_back(sql);  
}*/