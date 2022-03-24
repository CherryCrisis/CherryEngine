using CCEngine;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		Transform transform;

		public CameraController(System.IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn)
		{
			transform = GetComponent<Transform>();
		}

		public void Start()
        {
			
		}

		void SetRotation()
		{
			//transform.eulerAngles = new Vector3(transform.eulerAngles.x, CherryEngine.Sin(time), transform.eulerAngles.z);
			//transform.eulerAngles = new Vector3(transform.eulerAngles.x, transform.eulerAngles.y, CherryEngine.Sin(time));

		}

		public float deltaTime = 0.01f;
		float time = 0f;
		public void Update()
		{
			time += deltaTime;

			SetRotation();

			if (InputManager.GetInstance().GetKey(Keycode.SPACE))
				transform.position = new Vector3(transform.position.x, transform.position.y + 2f, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.LEFT_CONTROL))
				transform.position = new Vector3(transform.position.x, transform.position.y - 2f, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.R))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z + 2f);

			if (InputManager.GetInstance().GetKey(Keycode.F))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z - 2f);
		}
	}
}
