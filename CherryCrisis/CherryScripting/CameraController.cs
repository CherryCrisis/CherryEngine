﻿using CCEngine;

namespace CCScripting
{
	public class CameraController : Behaviour
	{
		Transform transform;

		public CameraController(System.IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn)
		{
			transform = GetComponent<Transform>();

		}

		DebugTest debug;

		public void Start()
        {
			debug = AddComponent<DebugTest>();
		}
		public string camName = "Je suis un nom";
		public int num = 5;

		public float deltaTime = 0.01f;
		float time = 0f;
		public void Update()
		{
			time += deltaTime;

			Vector2 deltaMouse = InputManager.GetInstance().GetMouseDelta();
			float sensitityX = Time.GetInstance().GetDeltaTime() * deltaMouse.x;
			float sensitityY = Time.GetInstance().GetDeltaTime() * deltaMouse.y;
			float speed = Time.GetInstance().GetDeltaTime() * 2f;

			transform.eulerAngles = new Vector3(transform.eulerAngles.x + sensitityY, transform.eulerAngles.y + sensitityX, transform.eulerAngles.z);

			if (InputManager.GetInstance().GetKey(Keycode.SPACE))
				transform.position = new Vector3(transform.position.x, transform.position.y + speed, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.LEFT_CONTROL))
				transform.position = new Vector3(transform.position.x, transform.position.y - speed, transform.position.z);

			if (InputManager.GetInstance().GetKey(Keycode.R))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z + speed);

			if (InputManager.GetInstance().GetKey(Keycode.F))
				transform.position = new Vector3(transform.position.x, transform.position.y, transform.position.z - speed);

			debug?.Debugger();
		}
	}
}
