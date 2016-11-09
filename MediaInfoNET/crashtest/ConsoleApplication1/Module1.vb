Imports MediaInfoNET

Module Module1

    Sub Main()
        Dim aviFile As MediaFile = New MediaFile("S09E15.mkv")

        Console.WriteLine()
        Console.WriteLine("General ---------------------------------")
        Console.WriteLine()
        Console.WriteLine("File Name   : {0}", aviFile.Name)
        Console.WriteLine("Format      : {0}", aviFile.General.Format)
        Console.WriteLine("Duration    : {0}", aviFile.General.DurationString)
        Console.WriteLine("Bitrate     : {0}", aviFile.General.Bitrate)

        If aviFile.Audio.Count > 0 Then
            Console.WriteLine()
            Console.WriteLine("Audio ---------------------------------")
            Console.WriteLine()
            Console.WriteLine("Format      : {0}", aviFile.Audio(0).Format)
            Console.WriteLine("Bitrate     : {0}", aviFile.Audio(0).Bitrate.ToString())
            Console.WriteLine("Channels    : {0}", aviFile.Audio(0).Channels.ToString())
            Console.WriteLine("Sampling    : {0}", aviFile.Audio(0).SamplingRate.ToString())
        End If

        If aviFile.Video.Count > 0 Then
            Console.WriteLine()
            Console.WriteLine("Video ---------------------------------")
            Console.WriteLine()
            Console.WriteLine("Format      : {0}", aviFile.Video(0).Format)
            Console.WriteLine("Bit rate    : {0}", aviFile.Video(0).Bitrate.ToString())
            Console.WriteLine("Frame rate  : {0}", aviFile.Video(0).FrameRate.ToString())
            Console.WriteLine("Frame size  : {0}", aviFile.Video(0).FrameSize.ToString())
        End If

        ''Console.ReadLine()
    End Sub
End Module

