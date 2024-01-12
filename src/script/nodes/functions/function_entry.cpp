// This file is part of the Godot Orchestrator project.
//
// Copyright (c) 2023-present Vahera Studios LLC and its contributors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//		http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "function_entry.h"

#include "script/function.h"

class OScriptNodeFunctionEntryInstance : public OScriptNodeInstance
{
    DECLARE_SCRIPT_NODE_INSTANCE(OScriptNodeFunctionEntry);
    size_t _argument_count{ 0 };

public:
    int step(OScriptNodeExecutionContext& p_context) override
    {
        for (size_t i = 0; i < _argument_count; i++)
            p_context.copy_input_to_output(i, i);
        return 0;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

OScriptNodeFunctionEntry::OScriptNodeFunctionEntry()
{
    _flags = ScriptNodeFlags::NONE;
}

void OScriptNodeFunctionEntry::allocate_default_pins()
{
    create_pin(PD_Output, "ExecOut")->set_flags(OScriptNodePin::Flags::EXECUTION);

    Ref<OScriptFunction> function = get_function();
    if (function.is_valid())
        create_pins_for_function_entry_exit(function, true);

    super::allocate_default_pins();
}

String OScriptNodeFunctionEntry::get_node_title() const
{
    Ref<OScriptFunction> function = get_function();
    if (!function.is_valid())
        return super::get_node_title();

    return function->get_function_name().capitalize();
}

String OScriptNodeFunctionEntry::get_tooltip_text() const
{
    Ref<OScriptFunction> function = get_function();
    if (function.is_valid())
        return vformat("Target is %s", get_owning_script()->get_base_type());

    return super::get_tooltip_text();
}

void OScriptNodeFunctionEntry::post_paste_node()
{
    super::post_paste_node();
    reconstruct_node();
}

bool OScriptNodeFunctionEntry::can_create_user_defined_pin(EPinDirection p_direction, String& r_message)
{
    bool result = super::can_create_user_defined_pin(p_direction, r_message);
    if (result)
    {
        if (p_direction == PD_Input)
        {
            r_message = "Cannot add input pins on a function entry node.";
            return false;
        }
    }
    return result;
}

OScriptNodeInstance* OScriptNodeFunctionEntry::instantiate(OScriptInstance* p_instance)
{
    OScriptNodeFunctionEntryInstance* i = memnew(OScriptNodeFunctionEntryInstance);
    i->_node = this;
    i->_instance = p_instance;
    i->_argument_count = _function->get_argument_count();
    return i;
}

void OScriptNodeFunctionEntry::initialize(const OScriptNodeInitContext& p_context)
{
    ERR_FAIL_COND_MSG(!p_context.method, "Failed to initialize node without a MethodInfo");

    const MethodInfo& mi = p_context.method.value();
    _function = get_owning_script()->create_function(mi, get_id(), _is_user_defined());
    _guid = _function->get_guid();

    super::initialize(p_context);
}
