namespace syn
{
    using System.Collections.Generic;

    public class Target
    {
        public IAdapter adapter;
        public object obj;

        public Target(IAdapter adapter, object obj)
        {
            this.adapter = adapter;
            this.obj = obj;
        }
    }

    public class Player
    {
        public List<Target> targets;
        public List<byte[]> sequences;
        public List<byte[]> userDataBlocks;
    }
}
