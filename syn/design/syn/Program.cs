namespace syn
{
    using System;
    using System.IO;

    using System.Runtime.InteropServices;

    public struct HANDLE
    {
        private int value;
    }

    public class WinApi
    {
        [DllImport("Kernel32.dll")]
        public static extern HANDLE LoadLibrary(string lpFileName);

        [DllImport("Kernel32.dll")]
        public static extern Delegate GetProcAddress(HANDLE hModule, string lpProcName);

    }

    class Program
    {
        /// <summary>bank configuration</summary>
        /// <remarks>
        /// - number of instruments
        ///  - instrument description as binary data
        /// </remarks>
        private static readonly byte[] bankConfig = { 0x02,  0x00, 0x01, 0x03, 0xff,  0x00, 0x03, 0xff };

        /// <summary>synth configuration</summary>
        /// <remarks>
        /// - number of synths
        /// - synth configurations: voice count
        /// </remarks>
        private static readonly byte[] synthConfig = { 0x02, 0x01, 0x03 };

        /// <summary>
        /// Main function
        /// </summary>
        /// <param name="args">command line arguments</param>
        public static void Main(string[] args)
        {
            // *************************************
            // Adapter: SynthAdapter
            // Sequence: importFromXM
            // UserDataBlock: adapter init data
            // *************************************
            RegisterPlugins();

            var asu = CreateSong("min.xm");
            if (asu.Prepare() == 0)
            {
                asu.Run();
            }
            
        }

        /// <summary>
        /// Read DLL files from plugin folder
        /// Activate each plugin by calling the entry function
        /// Register adapters defined by the plugins
        /// </summary>
        private static void RegisterPlugins()
        {
            // Get adapter plugins
            foreach (var dll in Directory.GetFiles("plugins", "*.dll"))
            {
                // winapi stuff
                var lib = WinApi.LoadLibrary(dll);
                // implicit call to DllMain
                var getAdapterInfos = (AdapterLib.ALGAI)WinApi.GetProcAddress(lib, "GetAdapterInfos");
                if (getAdapterInfos != null)
                {
                    var adapterInfos = getAdapterInfos();
                    foreach (var adapterInfo in adapterInfos)
                    {
                        Asu.RegisterAdapter(adapterInfo);
                    }
                }
            }
        }

        /// <summary>
        /// create song as an ASU document
        /// </summary>
        /// <param name="path">path to an XM tracker file</param>
        /// <returns>new ASU document</returns>
        static private Asu CreateSong(string path)
        {
            var asu = new Asu();
            var adapterInfo = Asu.GetAdapterInfo(SynthAdapter.Id);
            byte[] data = null;
            var length = CreateInitData(out data);
            var udb = asu.AddUserDataBlock(length, data);
            asu.AddAdapter(adapterInfo.id, udb);
            asu.ImportFromXm(path);
            return asu;
        }

        /// <summary>
        /// Create initialization data for the SynthAdapter
        /// </summary>
        /// <param name="data">byte array of initialization data</param>
        /// <returns>length of the created data</returns>
        private static int CreateInitData(out byte[] data)
        {
            var length = bankConfig.Length + synthConfig.Length;
            data = new byte[length];
            Buffer.BlockCopy(bankConfig, 0, data, 0, bankConfig.Length);
            Buffer.BlockCopy(synthConfig, bankConfig.Length, data, 0, synthConfig.Length);
            return length;
        }

    }
}


