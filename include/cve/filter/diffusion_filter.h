/* diffusion_filter.h
 *
 * Author: Fabian Meyer
 * Created On: 05 Sep 2019
 */

#ifndef CVE_DIFFUSION_FILTER_H_
#define CVE_DIFFUSION_FILTER_H_

#include "cve/filter/forward_differences_filter.h"
#include "cve/filter/backward_differences_filter.h"
#include "cve/filter/central_differences_filter.h"
#include "cve/filter/sobel_filter.h"

namespace cve
{
    template<typename Scalar>
    class GaussianDiffusivity
    {
    private:
        Scalar lambda_;
    public:
        GaussianDiffusivity()
            : GaussianDiffusivity(30)
        { }

        GaussianDiffusivity(const Scalar lambda)
            : lambda_(lambda)
        { }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            destImg = (-srcImg / srcImg.constant(lambda_ * lambda_)).exp();
        }
    };

    template<typename Scalar>
    class TotalVariationDiffusivity
    {
    public:
        TotalVariationDiffusivity()
        { }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            destImg = srcImg.constant(1) / srcImg.sqrt();
        }
    };

    template<typename Scalar,
        typename Diffusivity=GaussianDiffusivity<Scalar>,
        typename GradientFilter=SobelFilter<Scalar>>
    class DiffusionFilter
    {
    private:
        GradientFilter gradientFilter_;
        Diffusivity diffusivity_;

        Index maxIt_;
        Scalar flowFac_;
    public:
        DiffusionFilter()
            : DiffusionFilter(10, 0.05)
        { }

        DiffusionFilter(const Index iterations, const Scalar flowFactor)
            : gradientFilter_(), diffusivity_(), maxIt_(iterations),
            flowFac_(flowFactor)
        { }

        void setMaxIterations(const Index iterations)
        {
            maxIt_ = iterations;
        }

        void setFlowFactor(const Scalar flowFactor)
        {
            flowFac_ = flowFactor;
        }

        void setGradientFilter(const GradientFilter &filter)
        {
            gradientFilter_ = filter;
        }

        void setDiffusivity(const Diffusivity &diffusivity)
        {
            diffusivity_ = diffusivity;
        }

        template<typename ScalarA>
        void operator()(Eigen::Tensor<ScalarA, 3> &img) const
        {
            Eigen::Tensor<ScalarA, 3> tmp;
            operator()(img, tmp);
            img = tmp;
        }

        template<typename ScalarA, typename ScalarB>
        void operator()(const Eigen::Tensor<ScalarA, 3> &srcImg,
            Eigen::Tensor<ScalarB, 3> &destImg) const
        {
            Eigen::Tensor<Scalar, 3> u = srcImg.template cast<Scalar>();
            Eigen::Tensor<Scalar, 3> ux;
            Eigen::Tensor<Scalar, 3> uy;
            Eigen::Tensor<Scalar, 3> uMag;

            Eigen::Tensor<Scalar, 3> g;
            Eigen::Tensor<Scalar, 3> gux;
            Eigen::Tensor<Scalar, 3> guxx;
            Eigen::Tensor<Scalar, 3> guy;
            Eigen::Tensor<Scalar, 3> guyy;

            for(Index i = 0; i < maxIt_; ++i)
            {
                gradientFilter_(u, ux, uy);

                image::magnitudeSq(ux, uy, uMag);
                diffusivity_(uMag, g);

                gux = g * ux;
                guy = g * uy;

                gradientFilter_.applyX(gux, guxx);
                gradientFilter_.applyY(guy, guyy);

                u += flowFac_ * (guxx + guyy);
            }

            destImg = u.template cast<ScalarB>();
        }
    };
}

#endif
