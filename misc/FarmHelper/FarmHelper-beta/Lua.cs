using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Magic;

namespace FarmHelper_beta
{/*
    public class Lua
    {

        private BlackMagic wow { get; set; }
        private uint State;

        public Lua(BlackMagic _BlackMagic)
        {
            this.wow = _BlackMagic;
            State = GetState();
        }


        private enum Offsets : uint
        {
            GetLuaState = 0x0077D0F0,
            lua_gettop = 0x007ADAD0,
            lua_tostring = 0x007ADFE0,
        }

        public uint GetState()
        {
            uint codeCave = wow.AllocateMemory(0x1048);

            wow.Asm.Clear();
            wow.Asm.AddLine("call {0}", (uint)Offsets.GetLuaState);
            wow.Asm.AddLine("retn");

            uint result = wow.Asm.InjectAndExecute(codeCave);
            wow.FreeMemory(codeCave);
            return result;
        }

        public int GetTop()
        {
            uint codeCave = wow.AllocateMemory(0x1048);
            wow.Asm.Clear();
            wow.Asm.AddLine("push {0}", State);
            wow.Asm.AddLine("call {0}", (uint)Offsets.lua_gettop);
            wow.Asm.AddLine("add esp, 0x4");
            wow.Asm.AddLine("retn");

            uint result = wow.Asm.InjectAndExecute(codeCave);
            wow.FreeMemory(codeCave);
            return (int)result;
        }

        public override string ToString()
        {
            return ToString(GetTop(), 64);
        }

        public string ToString(int index, int length)
        {
            uint codeCave = wow.AllocateMemory(0x2048);

            wow.Asm.Clear();
            wow.Asm.AddLine("push 0");
            wow.Asm.AddLine("push {0}", index);
            wow.Asm.AddLine("push {0}", State);
            wow.Asm.AddLine("call {0}", (uint)Offsets.lua_tostring);
            wow.Asm.AddLine("add esp, 0xC");
            wow.Asm.AddLine("retn");

            uint result = wow.Asm.InjectAndExecute(codeCave);
            wow.FreeMemory(codeCave);

            try
            {
                return wow.ReadASCIIString(result, length);
            }
            catch (Exception e)
            {
                return "";
            }
        }

        public void Lua_DoString(string pszString)
        {
            ObjectManager __ObjectManager = new ObjectManager(this.wow);
            ObjectManager.WoWObjectManager _ObjectManager = __ObjectManager.GetWowObjectManager();

            wow.Asm.Clear();
            uint pScript = wow.AllocateMemory(pszString.Length + 1);
            wow.WriteASCIIString(pScript, pszString);

            wow.Asm.AddLine("mov EDX, [{0}]", _ObjectManager.g_clientConnection);   //Start UpdateCurMgr
            wow.Asm.AddLine("mov EDX, [EDX+{0}]", _ObjectManager.Offset);
            wow.Asm.AddLine("FS mov EAX, [0x2C]");
            wow.Asm.AddLine("mov EAX, [EAX]");
            wow.Asm.AddLine("add EAX, 8");
            wow.Asm.AddLine("mov [EAX], EDX"); // End UpdateCurMgr

            wow.Asm.AddLine("mov ecx, [{0}]", State);//0x00FC54EC);
            wow.Asm.AddLine("push ecx");
            wow.Asm.AddLine("mov eax, {0}", pScript);
            wow.Asm.AddLine("push eax");
            wow.Asm.AddLine("mov edx, {0}", pScript);
            wow.Asm.AddLine("push edx");

            wow.Asm.AddLine("call {0}", WowDecompile.FindOffset(this.wow, WowDecompile.Offsets.Lua_DoString));
            wow.Asm.AddLine("add esp, 0xC");
            wow.Asm.AddLine("retn");

            uint codeCave = wow.AllocateMemory(0x2048);
            wow.Asm.InjectAndExecuteEx(this.wow.ProcessHandle, codeCave);
        }

    }*/
}
