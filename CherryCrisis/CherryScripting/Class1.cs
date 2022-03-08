using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CCEngine;

namespace CherryScripting
{ 
	public class MyComponent : Behaviour
	{
		public MyComponent(System.IntPtr cPtr, bool cMemoryOwn) : base(cPtr, cMemoryOwn) {  }

		float i = 0.5f;

		Transform transform;

		public void Start()
        {
			transform = host.GetTransform();

			transform.position = new Vector3(transform.position.x, transform.position.y, -50f);
		}

		public void Update()
		{
			i += 0.1f;
			transform.eulerAngles = new Vector3(transform.eulerAngles.x, i, transform.eulerAngles.z);
		}
	}
}
