// Copyright (c) 2010-2020, Lawrence Livermore National Security, LLC. Produced
// at the Lawrence Livermore National Laboratory. All Rights reserved. See files
// LICENSE and NOTICE for details. LLNL-CODE-806117.
//
// This file is part of the MFEM library. For more information and source code
// availability visit https://mfem.org.
//
// MFEM is free software; you can redistribute it and/or modify it under the
// terms of the BSD-3 license. We welcome feedback and contributions, see file
// CONTRIBUTING.md for details.

#include "diffusion.hpp"

#include "../../config/config.hpp"
#ifdef MFEM_USE_CEED
#include "diffusion.h"
#endif

namespace mfem
{

CeedPADiffusionIntegrator::CeedPADiffusionIntegrator(
   const FiniteElementSpace &fes,
   const mfem::IntegrationRule &irm,
   Coefficient *Q)
   : CeedPAIntegrator()
{
#ifdef MFEM_USE_CEED
   Mesh &mesh = *fes.GetMesh();
   // Perform checks for some assumptions made in the Q-functions.
   MFEM_VERIFY(mesh.Dimension() == mesh.SpaceDimension(), "case not supported");
   MFEM_VERIFY(fes.GetVDim() == 1 || fes.GetVDim() == mesh.Dimension(),
               "case not supported");
   int dim = mesh.Dimension();
   CeedPAOperator diffOp = {fes, irm,
                            dim * (dim + 1) / 2, "/diffusion.h",
                            ":f_build_diff_const", f_build_diff_const,
                            ":f_build_diff_quad", f_build_diff_quad,
                            "", nullptr,
                            "", nullptr,
                            ":f_apply_diff", f_apply_diff,
                            EvalMode::Grad,
                            EvalMode::Grad
                           };
   DiffusionContext ctx;
   InitCeedCoeff(Q, mesh, irm, coeff, ctx);
   Assemble(diffOp, ctx);
#else
   mfem_error("MFEM must be built with MFEM_USE_CEED=YES to use libCEED.");
#endif
}

CeedMFDiffusionIntegrator::CeedMFDiffusionIntegrator(
   const FiniteElementSpace &fes,
   const mfem::IntegrationRule &irm,
   Coefficient *Q)
   : CeedMFIntegrator()
{
#ifdef MFEM_USE_CEED
   Mesh &mesh = *fes.GetMesh();
   CeedMFOperator diffOp = {fes, irm,
                            "/diffusion.h",
                            ":f_apply_diff_mf_const", f_apply_diff_mf_const,
                            ":f_apply_diff_mf_quad", f_apply_diff_mf_quad,
                            "", nullptr,
                            "", nullptr,
                            EvalMode::Grad,
                            EvalMode::Grad
                           };
   DiffusionContext ctx;
   InitCeedCoeff(Q, mesh, irm, coeff, ctx);
   Assemble(diffOp, ctx);
#else
   mfem_error("MFEM must be built with MFEM_USE_CEED=YES to use libCEED.");
#endif
}

} // namespace mfem
