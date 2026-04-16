    function targMap = targDataMap(),

    ;%***********************
    ;% Create Parameter Map *
    ;%***********************
    
        nTotData      = 0; %add to this count as we go
        nTotSects     = 2;
        sectIdxOffset = 0;

        ;%
        ;% Define dummy sections & preallocate arrays
        ;%
        dumSection.nData = -1;
        dumSection.data  = [];

        dumData.logicalSrcIdx = -1;
        dumData.dtTransOffset = -1;

        ;%
        ;% Init/prealloc paramMap
        ;%
        paramMap.nSections           = nTotSects;
        paramMap.sectIdxOffset       = sectIdxOffset;
            paramMap.sections(nTotSects) = dumSection; %prealloc
        paramMap.nTotData            = -1;

        ;%
        ;% Auto data (rtP)
        ;%
            section.nData     = 22;
            section.data(22)  = dumData; %prealloc

                    ;% rtP.C2
                    section.data(1).logicalSrcIdx = 0;
                    section.data(1).dtTransOffset = 0;

                    ;% rtP.Q_nom
                    section.data(2).logicalSrcIdx = 1;
                    section.data(2).dtTransOffset = 1;

                    ;% rtP.R0_2d_fix
                    section.data(3).logicalSrcIdx = 2;
                    section.data(3).dtTransOffset = 2;

                    ;% rtP.R2
                    section.data(4).logicalSrcIdx = 3;
                    section.data(4).dtTransOffset = 38;

                    ;% rtP.SoC_init
                    section.data(5).logicalSrcIdx = 4;
                    section.data(5).dtTransOffset = 39;

                    ;% rtP.Ts
                    section.data(6).logicalSrcIdx = 5;
                    section.data(6).dtTransOffset = 40;

                    ;% rtP.inv_C1_2d_fixed
                    section.data(7).logicalSrcIdx = 6;
                    section.data(7).dtTransOffset = 41;

                    ;% rtP.neg_inv_R1C1_2d_fixed
                    section.data(8).logicalSrcIdx = 7;
                    section.data(8).dtTransOffset = 77;

                    ;% rtP.ocv_tbl
                    section.data(9).logicalSrcIdx = 8;
                    section.data(9).dtTransOffset = 113;

                    ;% rtP.soc_bp
                    section.data(10).logicalSrcIdx = 9;
                    section.data(10).dtTransOffset = 134;

                    ;% rtP.soc_common
                    section.data(11).logicalSrcIdx = 10;
                    section.data(11).dtTransOffset = 155;

                    ;% rtP.temp_bp
                    section.data(12).logicalSrcIdx = 11;
                    section.data(12).dtTransOffset = 167;

                    ;% rtP.SoC_Integrator_gainval
                    section.data(13).logicalSrcIdx = 12;
                    section.data(13).dtTransOffset = 170;

                    ;% rtP.SoC_Limit_UpperSat
                    section.data(14).logicalSrcIdx = 13;
                    section.data(14).dtTransOffset = 171;

                    ;% rtP.SoC_Limit_LowerSat
                    section.data(15).logicalSrcIdx = 14;
                    section.data(15).dtTransOffset = 172;

                    ;% rtP.FromWorkspace_Time0
                    section.data(16).logicalSrcIdx = 15;
                    section.data(16).dtTransOffset = 173;

                    ;% rtP.FromWorkspace_Data0
                    section.data(17).logicalSrcIdx = 16;
                    section.data(17).dtTransOffset = 14172;

                    ;% rtP.Vp1_Integrator1_gainval
                    section.data(18).logicalSrcIdx = 17;
                    section.data(18).dtTransOffset = 28171;

                    ;% rtP.Vp1_Integrator1_IC
                    section.data(19).logicalSrcIdx = 18;
                    section.data(19).dtTransOffset = 28172;

                    ;% rtP.Vp2_Integrator_gainval
                    section.data(20).logicalSrcIdx = 19;
                    section.data(20).dtTransOffset = 28173;

                    ;% rtP.Vp2_Integrator_IC
                    section.data(21).logicalSrcIdx = 20;
                    section.data(21).dtTransOffset = 28174;

                    ;% rtP.T_cell_Value
                    section.data(22).logicalSrcIdx = 21;
                    section.data(22).dtTransOffset = 28175;

            nTotData = nTotData + section.nData;
            paramMap.sections(1) = section;
            clear section

            section.nData     = 3;
            section.data(3)  = dumData; %prealloc

                    ;% rtP.R0_LUT_maxIndex
                    section.data(1).logicalSrcIdx = 22;
                    section.data(1).dtTransOffset = 0;

                    ;% rtP.C1_LUT_maxIndex
                    section.data(2).logicalSrcIdx = 23;
                    section.data(2).dtTransOffset = 2;

                    ;% rtP.R1C1_LUT_maxIndex
                    section.data(3).logicalSrcIdx = 24;
                    section.data(3).dtTransOffset = 4;

            nTotData = nTotData + section.nData;
            paramMap.sections(2) = section;
            clear section


            ;%
            ;% Non-auto Data (parameter)
            ;%


        ;%
        ;% Add final counts to struct.
        ;%
        paramMap.nTotData = nTotData;



    ;%**************************
    ;% Create Block Output Map *
    ;%**************************
    
        nTotData      = 0; %add to this count as we go
        nTotSects     = 1;
        sectIdxOffset = 0;

        ;%
        ;% Define dummy sections & preallocate arrays
        ;%
        dumSection.nData = -1;
        dumSection.data  = [];

        dumData.logicalSrcIdx = -1;
        dumData.dtTransOffset = -1;

        ;%
        ;% Init/prealloc sigMap
        ;%
        sigMap.nSections           = nTotSects;
        sigMap.sectIdxOffset       = sectIdxOffset;
            sigMap.sections(nTotSects) = dumSection; %prealloc
        sigMap.nTotData            = -1;

        ;%
        ;% Auto data (rtB)
        ;%
            section.nData     = 13;
            section.data(13)  = dumData; %prealloc

                    ;% rtB.kehulh2wu3
                    section.data(1).logicalSrcIdx = 0;
                    section.data(1).dtTransOffset = 0;

                    ;% rtB.ljosjquhii
                    section.data(2).logicalSrcIdx = 1;
                    section.data(2).dtTransOffset = 1;

                    ;% rtB.e3eeyyf5wy
                    section.data(3).logicalSrcIdx = 2;
                    section.data(3).dtTransOffset = 2;

                    ;% rtB.mfdtfolo3m
                    section.data(4).logicalSrcIdx = 3;
                    section.data(4).dtTransOffset = 3;

                    ;% rtB.lvxiepm0a3
                    section.data(5).logicalSrcIdx = 4;
                    section.data(5).dtTransOffset = 4;

                    ;% rtB.njs2105xii
                    section.data(6).logicalSrcIdx = 5;
                    section.data(6).dtTransOffset = 5;

                    ;% rtB.d2mux5pgvk
                    section.data(7).logicalSrcIdx = 6;
                    section.data(7).dtTransOffset = 6;

                    ;% rtB.big3stfbru
                    section.data(8).logicalSrcIdx = 7;
                    section.data(8).dtTransOffset = 7;

                    ;% rtB.mhl1hybhar
                    section.data(9).logicalSrcIdx = 8;
                    section.data(9).dtTransOffset = 8;

                    ;% rtB.jbsvi3ud4a
                    section.data(10).logicalSrcIdx = 9;
                    section.data(10).dtTransOffset = 9;

                    ;% rtB.ioavimueej
                    section.data(11).logicalSrcIdx = 10;
                    section.data(11).dtTransOffset = 10;

                    ;% rtB.edd3psk1lf
                    section.data(12).logicalSrcIdx = 11;
                    section.data(12).dtTransOffset = 11;

                    ;% rtB.n24bl5rrpa
                    section.data(13).logicalSrcIdx = 12;
                    section.data(13).dtTransOffset = 12;

            nTotData = nTotData + section.nData;
            sigMap.sections(1) = section;
            clear section


            ;%
            ;% Non-auto Data (signal)
            ;%


        ;%
        ;% Add final counts to struct.
        ;%
        sigMap.nTotData = nTotData;



    ;%*******************
    ;% Create DWork Map *
    ;%*******************
    
        nTotData      = 0; %add to this count as we go
        nTotSects     = 4;
        sectIdxOffset = 1;

        ;%
        ;% Define dummy sections & preallocate arrays
        ;%
        dumSection.nData = -1;
        dumSection.data  = [];

        dumData.logicalSrcIdx = -1;
        dumData.dtTransOffset = -1;

        ;%
        ;% Init/prealloc dworkMap
        ;%
        dworkMap.nSections           = nTotSects;
        dworkMap.sectIdxOffset       = sectIdxOffset;
            dworkMap.sections(nTotSects) = dumSection; %prealloc
        dworkMap.nTotData            = -1;

        ;%
        ;% Auto data (rtDW)
        ;%
            section.nData     = 3;
            section.data(3)  = dumData; %prealloc

                    ;% rtDW.jeh3n4o4tq
                    section.data(1).logicalSrcIdx = 0;
                    section.data(1).dtTransOffset = 0;

                    ;% rtDW.g2nxhtulxs
                    section.data(2).logicalSrcIdx = 1;
                    section.data(2).dtTransOffset = 1;

                    ;% rtDW.na3yvoldgm
                    section.data(3).logicalSrcIdx = 2;
                    section.data(3).dtTransOffset = 2;

            nTotData = nTotData + section.nData;
            dworkMap.sections(1) = section;
            clear section

            section.nData     = 7;
            section.data(7)  = dumData; %prealloc

                    ;% rtDW.glb3z2swig.LoggedData
                    section.data(1).logicalSrcIdx = 3;
                    section.data(1).dtTransOffset = 0;

                    ;% rtDW.kg3edejg25.TimePtr
                    section.data(2).logicalSrcIdx = 4;
                    section.data(2).dtTransOffset = 1;

                    ;% rtDW.gd0mp3yj00.LoggedData
                    section.data(3).logicalSrcIdx = 5;
                    section.data(3).dtTransOffset = 2;

                    ;% rtDW.p0xxvfssv0.LoggedData
                    section.data(4).logicalSrcIdx = 6;
                    section.data(4).dtTransOffset = 3;

                    ;% rtDW.dnkslxlunw.AQHandles
                    section.data(5).logicalSrcIdx = 7;
                    section.data(5).dtTransOffset = 4;

                    ;% rtDW.eq441zyepa.LoggedData
                    section.data(6).logicalSrcIdx = 8;
                    section.data(6).dtTransOffset = 5;

                    ;% rtDW.kccth0tgsi.AQHandles
                    section.data(7).logicalSrcIdx = 9;
                    section.data(7).dtTransOffset = 6;

            nTotData = nTotData + section.nData;
            dworkMap.sections(2) = section;
            clear section

            section.nData     = 1;
            section.data(1)  = dumData; %prealloc

                    ;% rtDW.pk5clu0nyh
                    section.data(1).logicalSrcIdx = 10;
                    section.data(1).dtTransOffset = 0;

            nTotData = nTotData + section.nData;
            dworkMap.sections(3) = section;
            clear section

            section.nData     = 1;
            section.data(1)  = dumData; %prealloc

                    ;% rtDW.oti50icfwb.PrevIndex
                    section.data(1).logicalSrcIdx = 11;
                    section.data(1).dtTransOffset = 0;

            nTotData = nTotData + section.nData;
            dworkMap.sections(4) = section;
            clear section


            ;%
            ;% Non-auto Data (dwork)
            ;%


        ;%
        ;% Add final counts to struct.
        ;%
        dworkMap.nTotData = nTotData;



    ;%
    ;% Add individual maps to base struct.
    ;%

    targMap.paramMap  = paramMap;
    targMap.signalMap = sigMap;
    targMap.dworkMap  = dworkMap;

    ;%
    ;% Add checksums to base struct.
    ;%


    targMap.checksum0 = 1548344874;
    targMap.checksum1 = 3894418177;
    targMap.checksum2 = 829326392;
    targMap.checksum3 = 4263970587;

