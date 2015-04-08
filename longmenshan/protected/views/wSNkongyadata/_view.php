<?php
/* @var $this WSNkongyadataController */
/* @var $data WSNkongyadata */
?>

<div class="view">

	<b><?php echo CHtml::encode($data->getAttributeLabel('DID')); ?>:</b>
	<?php echo CHtml::link(CHtml::encode($data->DID), array('view', 'id'=>$data->DID)); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('SID')); ?>:</b>
	<?php echo CHtml::encode($data->SID); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('DataTime')); ?>:</b>
	<?php echo CHtml::encode($data->DataTime); ?>
	<br />

<!--	<b>--><?php //echo CHtml::encode($data->getAttributeLabel('S1')); ?><!--:</b>-->
<!--	--><?php //echo CHtml::encode($data->S1); ?>
<!--	<br />-->

<!--	<b>--><?php //echo CHtml::encode($data->getAttributeLabel('S2')); ?><!--:</b>-->
<!--	--><?php //echo CHtml::encode($data->S2); ?>
<!--	<br />-->

	<b><?php echo CHtml::encode($data->getAttributeLabel('R1')); ?>:</b>
	<?php echo CHtml::encode($data->R1); ?>
	<br />

	<b><?php echo CHtml::encode($data->getAttributeLabel('R2')); ?>:</b>
	<?php echo CHtml::encode($data->R2); ?>
	<br />

	<?php /*
	<b><?php echo CHtml::encode($data->getAttributeLabel('IsWarning')); ?>:</b>
	<?php echo CHtml::encode($data->IsWarning); ?>
	<br />

	*/ ?>

</div>