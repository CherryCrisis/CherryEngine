using CCEngine;

namespace CCScripting
{
    public class MainMenuHandler : Behaviour
    {
        public MainMenuHandler(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        public void Start()
        {
            InputManager.SetCursorDisplayed();
            InputManager.SetUIFocus(true);
        } 
        
    }
}