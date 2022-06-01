using CCEngine;

namespace CCScripting
{
    public class SoundModifier : Behaviour
    {
        public SoundModifier(System.IntPtr cPtr, bool cMemoryOwn)
            : base(cPtr, cMemoryOwn) {}

        AudioEmitter emitter;

        //called at the start of the game
        public void Awake()
        {
            emitter = GetBehaviour<AudioEmitter>();
        }

        void Start()
		{
            emitter.SetPitch(0.5f); 

        }

        //called each tick 
        public void Update()
        {
            if (InputManager.GetKeyDown(Keycode.A))
                emitter.Stop();

            if (InputManager.GetKeyDown(Keycode.E))
                emitter.Play();
        }
    }
}