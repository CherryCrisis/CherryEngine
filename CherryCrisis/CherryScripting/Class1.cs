using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using CCEngine;

namespace CherryScripting
{ 
	public class MyComponent : Behaviour
	{

		Vector3 vec = Vector3.Zero;

		public MyComponent(Entity owner) 
			: base(owner)
		{
			Console.WriteLine("MyComponent()");
			Console.WriteLine(owner);
		}

		int i = 5;

		public void Update()
		{
			Console.WriteLine("Update");
			Console.WriteLine(host);

			//Console.WriteLine(host);
		}
	}
}
