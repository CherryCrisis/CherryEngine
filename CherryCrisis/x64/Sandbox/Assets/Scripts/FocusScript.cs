using CCEngine;

namespace CCScripting
{
    public class FocusScript : Behaviour
    {
        public FocusScript(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}


        public void OnClick() 
        {
            InputManager.SetCursorHidden();
            InputManager.SetUIFocus(false);
        }
    }
}